# Smart Order Execution Simulator

**Status:** Working initial C++20 strategy, risk, routing, and analytics library committed. Full event-driven backtesting remains planned.

## Implemented

- TWAP child-order schedule with integer remainder conservation.
- VWAP schedule from a caller-provided volume curve.
- Pre-trade quantity and notional checks.
- Best-venue selection using ask price plus per-unit fee.
- Fill analytics: filled quantity, fill ratio, weighted average price, fees, and signed slippage.
- CMake build, runnable demo, and dependency-free unit tests.

## Build and run

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/execution_demo
```

## Assumptions

Prices are integer ticks. The VWAP curve is a set of non-negative volume weights, not a predictive model. Venue selection currently chooses one best displayed ask and does not model depth, latency, partial fills, or market impact. Slippage is reported as execution average minus arrival price; positive values indicate worse buy-side execution under this simplified convention.

## Next engineering milestones

1. Add a parent/child order state machine and simulated venue fill responses.
2. Add a deterministic event-driven backtester and configurable latency/fees.
3. Add buy/sell-aware execution metrics and implementation shortfall.
4. Add CSV input/output and optional PostgreSQL persistence.
