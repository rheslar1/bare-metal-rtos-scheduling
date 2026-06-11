# Bare-Metal RTOS Scheduling Design Package

## Purpose

Multi-threaded FreeRTOS or Zephyr application with queues, mutexes, semaphores, interrupt handoff, and priority inversion notes.

This package defines the project as an implementation-ready embedded system. It covers system architecture, requirements, interface boundaries, runtime design, validation evidence, and phased delivery.

## Project Profile

| Field | Value |
| --- | --- |
| Repository | `rheslar1/bare-metal-rtos-scheduling` |
| Primary stack | C++17, C++ Design Patterns, SOLID, FreeRTOS, Zephyr, IPC, Mutexes, Semaphores, Priority inheritance |
| Review proof point | Real concurrency, deterministic task design, shared-resource safety, and explainable scheduler tradeoffs. |

## Artifacts

- [System Design](system-design.md)
- [Requirements](requirements.md)
- [Interface Control](interface-control.md)
- [Runtime Design](runtime-design.md)
- [Validation Plan](validation-plan.md)
- [Implementation Roadmap](implementation-roadmap.md)
- [Draw.io UML](diagrams/system-design.drawio)
- [PNG UML](diagrams/system-design.png)
