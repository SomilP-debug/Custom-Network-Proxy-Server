# docs/caching_policy.md

## Caching Policy

The proxy implements an **LRU (Least Recently Used) cache** for HTTP GET requests.

### What is Cached

* Only HTTP GET responses that satisfy:
- HTTP method: `GET`
- Status code: `200 OK` are cached
* HTTPS traffic is never cached

### Cache Key

* Normalized host + normalized path
* Absolute URLs are converted to canonical form

### Why LRU

* Keeps frequently accessed resources
* Bounded memory usage

### Thread Safety

* Cache access is protected by a mutex

### Cache Stampede

* Concurrent first-time requests may all miss cache
* This behavior is documented as a limitation

---

