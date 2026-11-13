# High-Performance Order Matching Engine

**Status:** Working C++20 matching core with price-time priority, indexed cancellation, IOC/FOK policies, tests, and CMake build. Performance tuning and benchmark measurement remain planned.

## Implemented

- Price-time priority: best price first, FIFO at each price level.
- Limit and market orders with full and partial fills.
- Good-till-cancelled, immediate-or-cancel, and fill-or-kill time-in-force.
- FOK preflight liquidity check; an unfillable FOK is rejected without changing the book.
- O(1)-average active-order lookup and iterator-indexed cancellation.
- Integer price ticks, trade records, and depth snapshots.
- Mutex-protected public operations for safe concurrent callers.
- Dependency-free tests for FIFO matching, partial fills, cancellation, IOC, and FOK.

## Build and run

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/matching_demo
```

## Design notes and limitations

Each price level stores a FIFO list. An unordered index maps active order IDs to their price level and list iterator, avoiding a full-book scan for cancellation. The FOK check scans eligible resting liquidity before execution; it favors correctness and is not yet optimized. Market orders are treated as immediate-or-cancel to prevent unpriced residual liquidity from resting.

The engine is single-instrument and in-memory. Replace/amend operations, randomized property testing, persistence, and latency benchmarks are future work. No measured performance claims are published yet.
