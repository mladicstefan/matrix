
<h1 align="center">⚙️ C++ Webserver from Scratch</h1>
<p align="center"><em>"just use express bro..."</em></p>

<p align="center">
  <img src="https://img.shields.io/badge/C++-17-blue?logo=cplusplus" />
  <img src="https://img.shields.io/badge/License-MIT-green" />
  <img src="https://img.shields.io/badge/Build-passing-brightgreen" />
</p>


## NOTE! This is a learning project and should not be used in production!

## What I Built (Learning Project - Not Production Ready!)

* **Event-driven architecture** – Exploring the reactor pattern to see how servers can handle lots of connections efficiently
* **Non-blocking I/O with epoll** – Learning Linux's event notification system and why it scales better than traditional blocking approaches  
* **Basic thread pool** – Experimenting with worker threads to understand the balance between parallelism and resource overhead
* **HTTP/1.1 implementation** – Getting hands-on with connection reuse and proper protocol handling
* **Custom buffer management** – Understanding memory patterns in network programming and avoiding unnecessary allocations
* **Edge-triggered epoll** – Diving into the performance differences between level-triggered and edge-triggered event handling

This was built to understand low-level networking concepts and concurrent I/O patterns. Definitely has rough edges and missing production concerns like TLS and rate limiting which is currently handled by NGINX acting as a reverse proxy.
---


## Performance
Metrics:
![stress](assets/stresstest.png)
Stress test using Go:
```bash
go get golang.org/x/time/rate
go run stresst.go
````
---

### Prerequisites

* Linux (uses `epoll`)
* C++17 compiler (`g++` or `clang++`)
* `make`
* 
### Building

```bash
cd src/
make
```

### Running
#### Without docker !!!(CHECK MAIN.CPP)!!!

```bash
./server <port> <source_directory>

# Example:
./server 8080 ../public
```

#### With docker (recommended)
```bash
./deploy.sh
```
---

## Docker Security Features

* Multi-stage build (removes build tools from runtime)
* Non-root user execution
* Minimal runtime dependencies only
* Proper init system (tini) for signal handling
* Restricted file permissions
* Clean package cache removal

## 📜 License

MIT License – see [LICENSE](./LICENSE) for details.
