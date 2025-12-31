# docs/architecture.md

## Architecture Overview

The proxy follows a classic **client → proxy → server** architecture.

### Components

* **main.cpp**: Socket setup, accept loop, thread creation
* **proxy.cpp**: Core request handling logic
* **http_parser**: Parses HTTP requests and headers
* **https_tunnel**: Handles CONNECT tunneling
* **lru_cache**: Stores HTTP GET responses
* **blocklist**: Enforces domain-based blocking
* **logger**: Thread-safe logging utility
* **metrics**: - Tracks request counts per host and prints summary on shutdown



### Request Flow (HTTP)

1. Client sends HTTP request
2. Proxy parses and normalizes request
3. Blocklist check
4. Cache lookup
5. Forward to server (if needed)
6. Cache response
7. Send response to client

### Request Flow (HTTPS)

1. Client sends CONNECT request
2. Proxy checks blocklist
3. TCP tunnel established
4. Encrypted data forwarded bidirectionally

---

