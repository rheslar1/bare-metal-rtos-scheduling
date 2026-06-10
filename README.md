# Bare-Metal RTOS Scheduling

Multi-threaded FreeRTOS or Zephyr application with queues, mutexes, semaphores, interrupt handoff, and priority inversion notes.

## Portfolio Purpose

This repository implements a host-testable scheduler comparison for bare-metal firmware and RTOS-style tasking. It shows a timer-driven cooperative superloop beside a Linux-threaded RTOS model with mutex and semaphore resource boundaries.

## Stack

- C++17
- C++ Design Patterns
- SOLID
- FreeRTOS
- Zephyr
- IPC
- Mutexes
- Semaphores
- Priority inheritance

## Quick Start

```bash
cmake -S . -B build
cmake --build build
./build/bare_metal_rtos_scheduling
ctest --test-dir build --output-on-failure
```

## Implementation Slices

- Bare-metal timer superloop with periodic task releases and deadline-jitter evidence.
- Linux-threaded RTOS model using `std::thread`, mutex-protected I2C work, and a counting semaphore for ADC ownership.
- Shared task definitions for control loop, ADC sampler, I2C IMU, and telemetry work.
- Schedule reports showing release, start, finish, priority, runner, and deadline status.
- CTest coverage for timer dispatch, thread/resource behavior, overload deadline risk, and report evidence.

## Evidence Target

Real concurrency, deterministic task design, shared-resource safety, and explainable scheduler tradeoffs.

## Remote

Intended public repository: https://github.com/rheslar1/bare-metal-rtos-scheduling
