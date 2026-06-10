# Bare-Metal RTOS Scheduling

Multi-threaded FreeRTOS or Zephyr application with queues, mutexes, semaphores, interrupt handoff, and priority inversion notes.

## Portfolio Purpose

This repository is an Embedded Systems project scaffold for the Rheslar portfolio. It is designed to become a hardware-backed project with build output, validation logs, and reviewable implementation evidence.

All generated Embedded Systems repos are C++17-first and are framed around C++ design patterns and SOLID design principles.

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

- C++17 starter executable that exposes the project identity, stack, and validation target.
- Small strategy-style readiness check that keeps the scaffold aligned with C++ design patterns.
- Architecture document with control boundaries, data flow, safety assumptions, and evidence plan.
- CTest smoke test that keeps source, docs, and CI files present as the repo grows.
- GitHub Actions workflow for configure, build, executable smoke run, and repository validation.

## Evidence Target

Real concurrency, deterministic task design, shared-resource safety, and explainable scheduler tradeoffs.

## Remote

Intended public repository: https://github.com/rheslar1/bare-metal-rtos-scheduling
