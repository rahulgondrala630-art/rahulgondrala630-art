# High-Performance Order Matching Engine

**Status:** Planned — implementation not yet started.

## Overview

Build an exchange-style, single-instrument limit order book and matching engine in C++20. The core objective is deterministic price-time-priority matching with a clearly specified order lifecycle and measurable performance.

## Functional scope

- Limit and market orders; subsequently add IOC and FOK time-in-force policies.
- Price-time priority: best price first, then FIFO arrival order at a price level.
- Submit, cancel, and (where rules permit) replace orders.
- Full and partial fills, trade records, and order acknowledgements.
- Explicit handling of invalid quantities, duplicate identifiers, and insufficient liquidity.
- Deterministic replay of an input order stream.

## Engineering design

- Encapsulate price levels and FIFO order queues behind a documented order-book interface.
- Compare suitable data structures (for example, ordered maps versus indexed price levels) using measured workloads rather than assumptions.
- Use RAII and explicit ownership; avoid raw owning pointers.
- Consider object pools only after profiling demonstrates allocation pressure.
- Start with a single-writer matching core to preserve deterministic ordering. Add concurrency at the ingress/egress boundary only with documented sequencing guarantees.

## Planned modules

- `Order`: identifier, side, type, price, quantity, time-in-force, sequence.
- `OrderBook`: bid/ask levels and per-level FIFO queues.
- `MatchingEngine`: validation, matching, fills, and book updates.
- `Trade` / `ExecutionReport`: immutable execution outputs.
- `Replay`: deterministic event input and output comparison.

## Validation and performance

- Unit tests for price priority, time priority, partial fills, cancellation, empty books, and boundary cases.
- Property-based or randomized tests against a simple reference model.
- Stress tests with repeatable generated order streams.
- Google Benchmark measurements for throughput and latency distributions (p50, p95, p99); publish hardware, compiler, flags, dataset, and methodology.
- Run AddressSanitizer, UndefinedBehaviorSanitizer, and ThreadSanitizer where applicable.

## Planned stack

C++20 · STL · CMake · Linux · GoogleTest · Google Benchmark · sanitizers

## Acceptance criteria

- Deterministic output for identical input streams.
- Correct matching behavior covered by automated tests.
- Reproducible benchmark harness with no invented performance claims.
- Documented invariants, complexity trade-offs, build steps, and limitations.

## Non-goals

This is an educational exchange simulator, not a live trading venue or production financial system.
