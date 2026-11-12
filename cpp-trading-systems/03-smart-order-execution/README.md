# Smart Order Execution Simulator

**Status:** Planned — implementation not yet started.

## Overview

Develop a C++20 execution simulator that accepts a parent order, applies risk checks, schedules child orders using execution strategies, routes them across simulated venues, and evaluates execution quality against historical or generated market data.

## Functional scope

- Parent and child order lifecycle with clear state transitions.
- TWAP scheduling and a basic VWAP participation strategy.
- Simulated venues with configurable fees, latency, available depth, and fill behavior.
- Smart order routing based on configurable price and cost inputs.
- Pre-trade controls: quantity limits, notional limits, position limits, and kill switch.
- Backtesting with deterministic event replay.
- Execution analytics: fill ratio, average execution price, slippage, fees, and implementation shortfall.

## Engineering design

- Separate strategy interfaces from execution, routing, venue simulation, and risk controls.
- Inject a clock and deterministic random seed for repeatable tests.
- Keep risk validation in the order path; rejected orders must not reach simulated venues.
- Define concurrency boundaries and ownership explicitly. Use worker threads only where parallelism is useful and safe.
- Persist optional runs and trade records to PostgreSQL after the core simulation is correct.

## Planned modules

- `strategy/`: TWAP and VWAP scheduling.
- `execution/`: parent/child order state machine.
- `routing/`: simulated venue selection.
- `risk/`: pre-trade limits and kill switch.
- `backtest/`: historical/replayed event runner.
- `analytics/`: slippage, fills, fees, and summary reports.
- `storage/`: optional PostgreSQL persistence.

## Validation and performance

- Unit tests for strategy schedules, state transitions, risk rejections, and routing decisions.
- Deterministic integration tests with fixed market data and random seeds.
- Backtest comparisons against simple baseline execution schedules.
- Report fill ratio, average price, slippage, fees, and runtime.
- Test failure paths, venue unavailability, partial fills, and cancellation.
- Benchmark only after correctness is established.

## Planned stack

C++20 · STL · CMake · Linux · GoogleTest · Google Benchmark · PostgreSQL (optional)

## Acceptance criteria

- Repeatable backtests with documented assumptions.
- Risk controls demonstrably block invalid orders.
- Execution metrics calculated from recorded fills.
- Strategy and venue interfaces are independently testable.

## Non-goals

This project is a simulation and research portfolio piece, not financial advice, a live brokerage connection, or a claim of profitable trading.
