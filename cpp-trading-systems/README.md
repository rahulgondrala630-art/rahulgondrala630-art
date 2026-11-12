# C++ Trading Systems Portfolio

A collection of independent C++20 portfolio projects exploring the engineering foundations of electronic trading systems.

> **Status:** Project specifications and implementation plans. These projects are not yet implemented. Benchmark figures and performance claims will be added only after reproducible measurements.

## Projects

1. [High-Performance Order Matching Engine](01-order-matching-engine/README.md) — limit order book, price-time priority, order lifecycle, memory efficiency, deterministic matching.
2. [Real-Time Market Data Engine](02-market-data-engine/README.md) — TCP/UDP ingestion, binary message decoding, event sequencing, order-book reconstruction, concurrent pipelines.
3. [Smart Order Execution Simulator](03-smart-order-execution/README.md) — TWAP/VWAP scheduling, simulated smart routing, risk controls, backtesting, execution analytics.

## Engineering principles

- Modern C++20, RAII, value semantics, clear ownership, and the Rule of Zero where practical.
- Explicit concurrency models and documented synchronization invariants.
- Linux-first development with CMake; portable interfaces where reasonable.
- Automated tests, sanitizers, reproducible benchmarks, and CI.
- Honest reporting: distinguish simulated exchange behavior from real market connectivity.

## Planned common toolchain

C++20 · CMake · GCC/Clang · Linux · GoogleTest · Google Benchmark · AddressSanitizer · UndefinedBehaviorSanitizer · ThreadSanitizer · GitHub Actions

## Suggested build order

Build and validate the matching engine first, then the market-data pipeline, and finally connect both concepts through the execution simulator.
