# Smart Order Execution Simulator

**Status:** Working C++20 execution foundation with TWAP/VWAP schedules, pre-trade risk checks, venue routing, simulated partial fills, parent-order lifecycle, analytics, tests, and CMake build. A full historical event-driven backtester remains planned.

## Implemented

- TWAP child-order schedule with exact integer quantity conservation.
- VWAP schedule from a caller-provided non-negative volume curve.
- Pre-trade quantity and notional checks.
- Venue selection using displayed ask plus per-unit fee, considering availability and displayed quantity.
- Simulated buy child execution with partial fills against displayed venue quantity.
- Parent-order state transitions: open, partially filled, filled, and cancelled.
- Fill analytics: filled quantity, fill ratio, weighted average price, fees, and signed slippage.
- CMake build, runnable demo, and dependency-free unit tests.

## Build and run

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/execution_demo
```

## Assumptions and limitations

Prices are integer ticks. The VWAP curve is a set of non-negative volume weights, not a predictive model. Venue selection currently chooses one best displayed ask and does not model order-book depth beyond one displayed quantity, network latency, market impact, or sell-side execution. Slippage is reported as execution average minus arrival price; positive values indicate worse buy-side execution under this simplified convention.

## Next engineering milestones

1. Add deterministic event-driven backtesting and configurable latency/fees.
2. Add buy/sell-aware routing and execution metrics, including implementation shortfall.
3. Add multiple-venue child-order splitting and cancellation/replacement behavior.
4. Add CSV market-data input/output and optional PostgreSQL persistence.

No profitability, live-broker integration, or measured performance claims are made.
