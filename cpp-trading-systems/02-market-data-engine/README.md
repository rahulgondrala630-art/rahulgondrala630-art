# Real-Time Market Data Processing Engine

**Status:** Working initial C++20 feed codec, sequence validator, side-aware top-of-book, SPSC queue, and POSIX TCP/UDP readers committed. Snapshot recovery and end-to-end service integration remain planned.

## Implemented

- Fixed-width 34-byte binary event format with big-endian integer encoding and side flag.
- Bounds-checked decoder with event-type, side, and field validation.
- Sequence validator identifying in-order, gap, and duplicate/old messages.
- In-memory order event application with best bid and best ask accessors.
- Bounded single-producer/single-consumer ring queue using acquire/release atomics.
- POSIX TCP length-prefixed frame reader and UDP datagram reader; caller owns socket lifecycle.
- CMake build, demonstration program, and dependency-free unit tests, including a TCP socket-pair test.

## Build and run

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/feed_demo
```

## Current protocol and concurrency contract

Each event contains sequence (8 bytes), event type (1), side (1), order ID (8), price ticks (8), and quantity (8), all multi-byte fields in network byte order. TCP uses a four-byte big-endian frame length. The SPSC queue is explicitly single-producer/single-consumer; it must not be used as an MPMC queue.

## Next engineering milestones

1. Add server/client lifecycle, reconnect behavior, socket timeouts, and UDP truncation handling.
2. Add gap buffering, snapshot recovery, and replay from captured feed files.
3. Connect transport to the bounded queue and order-book processor.
4. Add robust handling for modify/cancel/trade events, instrument identifiers, and load tests.

No network throughput or latency claims are made until measured on a documented environment.
