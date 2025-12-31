# docs/concurrency_model.md

## Concurrency Model

The proxy uses a **thread-per-client** concurrency model.

### Design

* Each accepted client connection is handled by a detached thread
* Threads operate independently
* Shared resources (cache, logs, blocklist) are protected by mutexes

### HTTPS Tunneling

For CONNECT requests:

* Two threads are created:

  * Client → Server
  * Server → Client
* Both threads are joined to ensure clean tunnel termination

### Why detach?

* The main server loop must not block
* Client lifecycle is independent

### Context Switching

* Threads are scheduled by the OS
* On a single core, threads run via time-slicing

---

