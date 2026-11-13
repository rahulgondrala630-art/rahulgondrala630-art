# High-Performance Order Matching Engine

**Status:** Working initial C++20 implementation committed. Further optimization and expanded order policies remain planned.

## Implemented

- Price-time priority matching for limit and market orders.
- Bid and ask price levels with FIFO queues at each price.
- Partial fills and trade records at the resting maker price.
- Active order ID validation, cancellation, and market-depth snapshots.
- Mutex-protected public operations for safe concurrent callers.
- CMake build, runnable demonstration, and dependency-free unit tests.

## Build and run

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/matching_demo
```

Prices are represented as integer ticks to avoid floating-point comparison errors. This first version favors clarity and deterministic behavior; cancellation currently scans price-level queues and should be replaced with an iterator index before making latency claims.

## Next engineering milestones

1. Add replace, IOC, and FOK order semantics with explicit lifecycle states.
2. Add iterator-based cancellation and allocation-aware order storage.
3. Add randomized/property tests, replay fixtures, and race testing.
4. Add a reproducible latency/throughput benchmark before publishing performance figures.

This is a single-instrument educational exchange simulator, not a production exchange or live trading system.
