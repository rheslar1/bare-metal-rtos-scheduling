# Bare-Metal RTOS Scheduling Architecture

## Goal

Real concurrency, deterministic task design, shared-resource safety, and explainable scheduler tradeoffs.

## Runtime Shape

1. A timer source releases periodic control, ADC, IMU, and telemetry work.
2. The bare-metal version dispatches tasks from a cooperative superloop.
3. The RTOS/Linux version maps each task to a thread with priority metadata.
4. Shared I2C access is protected by a mutex.
5. ADC conversion ownership is represented by a counting semaphore.
6. Schedule evidence records release/start/finish timing and deadline misses.

## C++17 Design Shape

- `BareMetalTimerSuperloop` models a timer ISR plus cooperative main loop.
- `LinuxThreadedRtosRunner` models RTOS-style periodic tasks with Linux threads.
- `CountingSemaphore` provides a C++17 semaphore stand-in for ADC ownership.
- `TextScheduleReporter` emits schedule evidence for CI and review.

## SOLID Notes

- Single Responsibility: timer scheduling, thread scheduling, semaphore ownership, and reporting are separated.
- Open/Closed: new task sets can be added without changing scheduler internals.
- Liskov Substitution: reports share a common evidence shape across bare-metal and RTOS runners.
- Interface Segregation: each runner exposes a focused `run` method.
- Dependency Inversion: tests validate scheduler behavior through public schedule reports.

## Boundaries

- `include/scheduling/`: task definitions, schedule reports, and scheduler interfaces.
- `src/`: bare-metal scheduler, Linux-threaded RTOS model, semaphore, and CLI demo.
- `docs/`: validation plans, timing notes, hardware captures, and acceptance evidence.
- `tests/`: host-side tests for timer, thread, mutex, semaphore, and deadline behavior.
- `.github/workflows/`: CI entry point for build and validation evidence.

## Validation Plan

- Build the host scheduler model with CMake.
- Run the executable and inspect both bare-metal and Linux-threaded reports.
- Run CTest to validate dispatch, resource protection, and overload behavior.
- Add FreeRTOS or Zephyr task trace logs after target hardware integration.
- Capture CI, terminal, and hardware evidence for the portfolio detail page.

## Expansion Notes

- Map the host `TaskSpec` records to FreeRTOS or Zephyr task creation calls.
- Replace virtual timer ticks with hardware timer ISR captures on target.
- Keep Linux-threaded tests as a fast regression surface for scheduling tradeoffs.

<!-- cpp17-solid-implementation:start -->
## C++17, Design Patterns, and SOLID Implementation

This repository includes a host-buildable C++17 implementation, not only documentation. The implementation applies:

- Strategy pattern for validation rules.
- Adapter interfaces for input samples and telemetry/reporting.
- Composite validation for combining safety and readiness checks.
- Facade orchestration through the project runtime class.
- SOLID boundaries between profile data, input acquisition, validation, telemetry encoding, and tests.
<!-- cpp17-solid-implementation:end -->

<!-- deep-architecture-links:start -->
## Deep Architecture and UML

- [Deep architecture](docs/deep-architecture.md)
- [Full UML Draw.io source](docs/diagrams/full-system-uml.drawio)
- [Full UML PNG export](docs/diagrams/full-system-uml.png)
<!-- deep-architecture-links:end -->
