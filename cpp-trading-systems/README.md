# C++ Trading Systems Portfolio

Three independent C++20 portfolio projects focused on core electronic-trading engineering.

## Current implementation status

| Project | Implemented baseline | Remaining major work |
| --- | --- | --- |
| [High-Performance Order Matching Engine](01-order-matching-engine/README.md) | Price-time matching, partial fills, cancellation, depth snapshots, tests | Iterator-indexed cancellation, order policies, benchmarks |
| [Real-Time Market Data Engine](02-market-data-engine/README.md) | Binary codec, validation, sequencing, basic book demo, tests | TCP/UDP sockets, side-aware protocol, recovery, concurrent pipeline |
| [Smart Order Execution Simulator](03-smart-order-execution/README.md) | TWAP/VWAP schedules, risk checks, venue selection, analytics, tests | Parent/child state machine, event-driven backtester, persistence |

These are portfolio implementations under active development. Features listed as remaining work are not yet implemented. No benchmark results or production-performance claims are published until measured.

## Build all projects

From this directory:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Requires CMake 3.20+ and a C++20 compiler (GCC 10+, Clang 12+, or equivalent).

## Engineering principles

- C++20, RAII, standard-library containers, and explicit ownership.
- Integer price ticks for deterministic financial arithmetic.
- Reproducible tests and documented assumptions.
- Correctness before optimization; publish performance numbers only with reproducible benchmark methodology.
