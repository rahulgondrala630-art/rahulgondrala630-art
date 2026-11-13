# Real-Time Market Data Processing Engine

**Status:** Working initial C++20 feed codec, sequence validator, side-aware top-of-book, bounded SPSC queue, and demos committed. Network transport and recovery remain planned.

## Implemented

- Fixed-width 34-byte binary event format with big-endian integer encoding and side flag.
- Bounds-checked decoder with event-type, side, and field validation.
- Sequence validator identifying in-order, gap, and duplicate/old messages.
- In-memory order event application with best bid and best ask accessors.
- Bounded single-producer/single-consumer ring queue using acquire/release atomics.
- CMake build, demonstration program, and dependency-free unit tests.

## Build and run

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/feed_demo
```

## Current protocol and concurrency contract

Each event contains sequence (8 bytes), event type (1), side (1), order ID (8), price ticks (8), and quantity (8), all multi-byte fields in network byte order. The current book maintains individual orders and derives the best bid/ask. The SPSC queue is explicitly single-producer/single-consumer; it must not be used as an MPMC queue.

## Next engineering milestones

1. Implement TCP stream framing and UDP datagram receivers on Linux.
2. Add gap buffering, snapshot recovery, and replay from captured feed files.
3. Connect the queue to an ingestion/processing pipeline and measure throughput/latency.
4. Add robust handling for modify/cancel/trade events and instrument identifiers.

No network throughput or latency claims are made until measured on a documented environment.
