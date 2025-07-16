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

func (s *Stats) recordSucess() {
	atomic.AddInt64(&s.successCount, 1)
	atomic.AddInt64(&s.totalRequests, 1)
}

func (s *Stats) recordError() {
	atomic.AddInt64(&s.errorCount, 1)
	atomic.AddInt64(&s.totalRequests, 1)
}

func reqWorker(
	ctx context.Context,
	url string,
	stats *Stats,
	limiter *rate.Limiter,
	wg *sync.WaitGroup) {

	defer wg.Done()
	for {
		select {
		case <-ctx.Done():
			return
		default:
			if err := limiter.Wait(ctx); err != nil {
				return
			}

			client := &http.Client{
				Timeout: 5 * time.Second,
				Transport: &http.Transport{
					DisableKeepAlives: false,
				},
			}

			resp, err := client.Get(url)
			if err != nil {
				stats.recordError()
				continue
			}
			resp.Body.Close()

			if resp.StatusCode >= 200 && resp.StatusCode < 300 {
				stats.recordSucess()
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
	const (
		numWorkers = 1000
		targetURL  = "http://localhost:8080"
		duration   = 30 * time.Second
		maxRPS     = 0 //set 0 for no rate limitng
	)
	stats := &Stats{startTime: time.Now()}
	ctx, cancel := context.WithTimeout(context.Background(), duration)
	defer cancel()
	// 0 == no rate limit

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
		go reqWorker(ctx, targetURL, stats, limiter, &wg)
	}
	rpsInfo := "unlimited"
	if maxRPS > 0 {
		rpsInfo = fmt.Sprintf("max %d RPS", maxRPS)
	}
	fmt.Printf("Starting stress test with %d workers against %s for %v (%s) [NEW CLIENT PER REQUEST]\n",
		numWorkers, targetURL, duration, rpsInfo)

	wg.Wait()
	fmt.Printf("\n\nFinal Results:\n")
	fmt.Printf("Total Requests: %d\n", atomic.LoadInt64(&stats.totalRequests))
	fmt.Printf("Successful: %d\n", atomic.LoadInt64(&stats.successCount))
	fmt.Printf("Errors: %d\n", atomic.LoadInt64(&stats.errorCount))
	fmt.Printf("Duration: %v\n", duration)
	fmt.Printf("Average RPS: %.2f\n", float64(atomic.LoadInt64(&stats.totalRequests))/duration.Seconds())

	total := atomic.LoadInt64(&stats.totalRequests)
	success := atomic.LoadInt64(&stats.successCount)
	if total > 0 {
		successPercent := float64(success) / float64(total) * 100
		fmt.Printf("Success Rate: %.2f%%\n", successPercent)
	} else {
		fmt.Printf("Success Rate: N/A (no requests made)\n")
	}

	if maxRPS > 0 {
		fmt.Printf("Rate Limit: %d RPS\n", maxRPS)
	}
}
