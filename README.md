# README.md

## Custom HTTP/HTTPS Proxy Server (C++)

**Video-Recording link** - https://drive.google.com/file/d/1PwWpocsb_ngd_QkXpCTevizuKfKWGwhD/view?usp=sharing

### Overview

This project implements a **fully functional HTTP/HTTPS proxy server** in C++ from scratch. The proxy supports standard HTTP requests (via `curl -x`) and HTTPS traffic using the `CONNECT` method with secure bidirectional tunneling. It is designed to demonstrate low-level network programming, concurrency, caching, and system-level design concepts.

The proxy acts as an **intermediary** between clients and remote servers:

* For **HTTP**, it parses, forwards, caches, and logs requests.
* For **HTTPS**, it establishes a transparent TCP tunnel without inspecting encrypted data.

---

## ✨ Key Features

### HTTP Proxying
- Supports standard HTTP methods (`GET`, `POST`, etc.)
- Handles both:
  - origin-form requests (`GET /path HTTP/1.1`)
  - absolute-form requests (`GET http://host/path HTTP/1.1`)
- Normalizes host, path, and port before forwarding
- Correctly forwards request headers and bodies

### HTTPS Proxying (CONNECT)
- Full support for `CONNECT host:port HTTP/1.1`
- Establishes a **bidirectional TCP tunnel**
- Compatible with browsers and `curl -x`
- Domain-level blocking for HTTPS requests

### Correct Content-Length Handling
- Reads request bodies **exactly** according to `Content-Length`
- Prevents partial reads and protocol desynchronization
- Essential for correct handling of POST/PUT requests

### Streaming Response Forwarding
- Responses are forwarded **incrementally as they are received**
- No requirement to buffer full responses in memory
- Enables low latency and efficient memory usage

### LRU Cache
- Thread-safe **Least Recently Used (LRU)** cache
- Caches only:
  - HTTP `GET` requests
  - Successful `200 OK` responses
- Cache key:

### Domain Blocking
- Configurable via `blocked_domains.txt`
- Blocks:
- HTTP requests
- HTTPS CONNECT requests
- Enforcement happens **before contacting the origin server**

### Logging
- Structured access logs with:
- Client IP
- Host
- Path or CONNECT
- Status code
- Bytes transferred
- Timestamp
- Uses Log Rotation Strategy

### Metrics
- Tracks request counts per host
- Summary printed during graceful shutdown
### Concurrency
- Thread-per-connection model
- Each client handled independently
- Shared resources protected using mutexes

### Graceful Shutdown
- Handles `SIGINT` and `SIGTERM`
- Stops accepting new connections
- Allows active connections to complete
- Flushes logs and prints metrics summary

---

### Build Instructions

```bash
make
```

This produces the executable `proxy` in the project root.

---

### Run

```bash
./proxy
```

The proxy listens on port **8000** by default.

---

### Testing

```bash
# HTTP
curl -x http://localhost:8000 http://example.com

# HTTPS
curl -x http://localhost:8000 https://google.com

# Blocking test
curl -x http://localhost:8000 http://blocked-domain.com
```

---

### Blocking Configuration

Edit `blocked_domains.txt` (project root):

```
facebook.com
youtube.com
```

Restart the proxy after changes.

---

### Logging

Access logs are written to `proxy.log` or `proxy.log.1` in the format:

```
[TIMESTAMP] CLIENT_IP HOST PATH|CONNECT STATUS_CODE BYTES_TRANSFERRED
```

---

### Limitations (Summary)

* No HTTP/2 support
* No TLS inspection or MITM
* HTTPS responses are not cached
* No chunked request decoding

---

