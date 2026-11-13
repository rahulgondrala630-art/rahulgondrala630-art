# C++ Trading Systems Portfolio

Three independent C++20 portfolio projects focused on core electronic-trading engineering.

## Current implementation status

| Project | Implemented now | Remaining major work |
| --- | --- | --- |
| [High-Performance Order Matching Engine](01-order-matching-engine/README.md) | Price-time priority, limit/market orders, partial fills, GTC/IOC/FOK, indexed cancellation, depth snapshots, tests | Replace/amend operations, property tests, benchmarks |
| [Real-Time Market Data Engine](02-market-data-engine/README.md) | Binary codec, sequence validation, side-aware top of book, SPSC queue, POSIX TCP/UDP readers, tests | Recovery/replay, end-to-end pipeline, reconnect handling, benchmarks |
| [Smart Order Execution Simulator](03-smart-order-execution/README.md) | TWAP/VWAP, pre-trade risk, venue selection, simulated partial fills, parent-order lifecycle, execution analytics, tests | Event-driven backtester, multi-venue splitting, sell-side routing, persistence |

Features listed as remaining work are not yet implemented. No benchmark results or production-performance claims are published until measured.

## Build all projects

From this directory:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Requires CMake 3.20+ and a C++20 compiler. The GitHub Actions workflow at `.github/workflows/cpp-trading-systems.yml` is configured to build and run tests on pushes and pull requests touching this portfolio.

## Engineering principles

- C++20, RAII, standard-library containers, and explicit ownership.
- Integer price ticks for deterministic financial arithmetic.
- Reproducible tests and documented assumptions.
- Correctness before optimization; publish performance numbers only with reproducible benchmark methodology.
