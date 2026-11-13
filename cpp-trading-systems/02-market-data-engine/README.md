# Real-Time Market Data Processing Engine

**Status:** Working initial C++20 feed codec, sequence validator, and local book demo committed. Network transport and recovery remain planned.

## Implemented

- Fixed-width 33-byte binary event format with big-endian integer encoding.
- Bounds-checked decoder with event-type and field validation.
- Sequence validator identifying in-order, gap, and duplicate/old messages.
- In-memory order event application and best bid/ask accessors.
- CMake build, demonstration program, and dependency-free unit tests.

## Build and run

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/feed_demo
```

## Current protocol

Each event contains sequence (8 bytes), event type (1), order ID (8), price ticks (8), and quantity (8), all multi-byte fields in network byte order. The current sample book treats added orders as bids; side-aware messages, snapshot recovery, and full depth aggregation are the next protocol revision.

## Next engineering milestones

1. Add side and instrument fields to the versioned wire protocol.
2. Implement TCP stream framing and UDP datagram receivers on Linux.
3. Add gap buffering, snapshot recovery, and replay from captured feed files.
4. Add a bounded producer-consumer pipeline and measure throughput/latency.

No network throughput or latency claims are made until measured on a documented environment.
