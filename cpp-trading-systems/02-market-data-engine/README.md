# Real-Time Market Data Processing Engine

**Status:** Planned — implementation not yet started.

## Overview

Create a Linux-oriented C++20 service that ingests a simulated market-data feed, decodes messages, maintains ordered events, reconstructs a local order book, and publishes market snapshots. Emphasis is on network programming, throughput, concurrency, and recovery behavior.

## Functional scope

- TCP stream and UDP datagram receiver components.
- Versioned binary message format with explicit byte order and bounds-checked decoding.
- Message types for add, modify, cancel, trade, and snapshot events.
- Sequence-number validation and detection of gaps, duplicates, and out-of-order updates.
- Snapshot-based recovery and deterministic feed replay.
- Reconstructed book and derived metrics such as spread, mid-price, and displayed depth.

## Engineering design

- Separate transport, decoding, validation, sequencing, book application, and publishing layers.
- Use non-blocking sockets or an event loop for network I/O; document backpressure behavior.
- Begin with a simple, correct bounded queue. Evaluate an SPSC lock-free ring buffer only when producer/consumer ownership assumptions are proven.
- Use atomics with explicit memory-order reasoning; avoid claiming a structure is lock-free without validating the implementation.
- Keep packet parsing allocation-conscious and bounds-safe.

## Planned modules

- `transport/`: TCP and UDP adapters.
- `protocol/`: message schema, encoder, and decoder.
- `sequencing/`: sequence tracking and gap detection.
- `pipeline/`: bounded queues and worker stages.
- `book/`: incremental order-book reconstruction.
- `metrics/`: throughput, queue depth, drops, and latency.

## Validation and performance

- Decoder tests for malformed, truncated, and endian-sensitive messages.
- Integration tests using a local feed simulator.
- Tests for packet loss, duplication, reordering, and sequence gaps.
- Load tests reporting messages/second and p50/p95/p99 end-to-end latency.
- Measure CPU utilization, queue depth, and dropped messages.
- Run sanitizers and repeatable benchmarks; publish environment and methodology.

## Planned stack

C++20 · POSIX sockets · TCP/UDP · Linux · STL · atomics · CMake · GoogleTest · Google Benchmark

## Acceptance criteria

- Correct decoding and sequencing under documented test scenarios.
- Recovery behavior for detectable feed gaps.
- Observable backpressure and dropped-message behavior.
- Reproducible performance measurements.

## Non-goals

Initial versions use a local simulator and do not connect to proprietary exchange feeds or claim exchange-grade latency.
