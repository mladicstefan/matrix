package main

import (
	"context"
	"fmt"
	"net/http"
	"sync"
	"sync/atomic"
	"time"

	"golang.org/x/time/rate"
)

type Stats struct {
	totalRequests int64
	successCount  int64
	errorCount    int64
	startTime     time.Time
}

func (s *Stats) recordSuccess() {
	atomic.AddInt64(&s.successCount, 1)
	atomic.AddInt64(&s.totalRequests, 1)
}

func (s *Stats) recordError() {
	atomic.AddInt64(&s.errorCount, 1)
	atomic.AddInt64(&s.totalRequests, 1)
}

func worker(ctx context.Context, client *http.Client, url string, stats *Stats, limiter *rate.Limiter, wg *sync.WaitGroup) {
	defer wg.Done()

	for {
		select {
		case <-ctx.Done():
			return
		default:
			// Wait for rate limiter permission
			if err := limiter.Wait(ctx); err != nil {
				return // Context cancelled
			}

			resp, err := client.Get(url)
			if err != nil {
				stats.recordError()
				continue
			}

			resp.Body.Close()

			if resp.StatusCode >= 200 && resp.StatusCode < 300 {
				stats.recordSuccess()
			} else {
				stats.recordError()
			}
		}
	}
}

func printStats(stats *Stats) {
	for {
		time.Sleep(1 * time.Second)

		total := atomic.LoadInt64(&stats.totalRequests)
		success := atomic.LoadInt64(&stats.successCount)
		errors := atomic.LoadInt64(&stats.errorCount)

		elapsed := time.Since(stats.startTime).Seconds()
		rps := float64(total) / elapsed

		fmt.Printf("\rRequests: %d | Success: %d | Errors: %d | RPS: %.2f",
			total, success, errors, rps)
	}
}

func main() {
	// Configuration
	const (
		numWorkers = 1000 // Number of concurrent goroutines
		targetURL  = "http://localhost:8080"
		duration   = 30 * time.Second // Test duration
		maxRPS     = 1000             // Maximum requests per second (0 = unlimited)
	)

	client := &http.Client{
		Timeout: 5 * time.Second,
		Transport: &http.Transport{
			MaxIdleConns:        numWorkers,
			MaxIdleConnsPerHost: numWorkers,
			MaxConnsPerHost:     numWorkers,
			DisableKeepAlives:   false,
		},
	}

	stats := &Stats{startTime: time.Now()}

	ctx, cancel := context.WithTimeout(context.Background(), duration)
	defer cancel()

	// Create rate limiter (0 = unlimited)
	var limiter *rate.Limiter
	if maxRPS > 0 {
		limiter = rate.NewLimiter(rate.Limit(maxRPS), maxRPS) // Allow bursts up to maxRPS
	} else {
		limiter = rate.NewLimiter(rate.Inf, 1) // Unlimited
	}

	go printStats(stats)

	var wg sync.WaitGroup
	for i := 0; i < numWorkers; i++ {
		wg.Add(1)
		go worker(ctx, client, targetURL, stats, limiter, &wg)
	}

	rpsInfo := "unlimited"
	if maxRPS > 0 {
		rpsInfo = fmt.Sprintf("max %d RPS", maxRPS)
	}

	fmt.Printf("Starting stress test with %d workers against %s for %v (%s)\n",
		numWorkers, targetURL, duration, rpsInfo)

	wg.Wait()

	fmt.Printf("\n\nFinal Results:\n")
	fmt.Printf("Total Requests: %d\n", atomic.LoadInt64(&stats.totalRequests))
	fmt.Printf("Successful: %d\n", atomic.LoadInt64(&stats.successCount))
	fmt.Printf("Errors: %d\n", atomic.LoadInt64(&stats.errorCount))
	fmt.Printf("Duration: %v\n", duration)
	fmt.Printf("Average RPS: %.2f\n", float64(atomic.LoadInt64(&stats.totalRequests))/duration.Seconds())
	if maxRPS > 0 {
		fmt.Printf("Rate Limit: %d RPS\n", maxRPS)
	}
}
