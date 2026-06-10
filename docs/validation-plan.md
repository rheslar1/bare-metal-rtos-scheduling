# Validation Plan

## Current Host Checks

- CMake configure completes.
- C++17 scheduler model builds.
- Executable prints both bare-metal timer and Linux-threaded RTOS schedule reports.
- CTest verifies timer dispatch, thread evidence, mutex/semaphore use, and overload deadline risk.
- GitHub Actions runs configure, build, executable smoke run, and CTest.

## Hardware Evidence To Add

- Hardware timer ISR capture from the bare-metal version.
- FreeRTOS or Zephyr trace showing task priorities and wakeups.
- Mutex contention log around shared I2C access.
- ADC semaphore handoff log from ISR to worker task.
- Deadline and jitter histogram from the target board.
- CI screenshot after the public repository is pushed.

## Project-Specific Evidence Target

Real concurrency, deterministic task design, shared-resource safety, and explainable scheduler tradeoffs.
