# Bare-Metal and RTOS/Linux Scheduling Notes

## Bare-Metal Version

- A hardware timer interrupt increments a tick counter.
- Periodic tasks are released from a cooperative superloop.
- Shared peripherals are protected by execution order and short critical sections.
- Long low-priority work can delay later releases because there is no preemption.

## RTOS/Linux Version

- Each periodic task is represented by a Linux thread in the host model.
- Timer releases are modeled as per-task periodic release points.
- Shared I2C work uses a mutex.
- ADC conversion ownership uses a counting semaphore.
- Priorities are captured in the schedule evidence so a FreeRTOS or Zephyr port can map them to native task priorities.

## Target RTOS Mapping

- `control-loop`: highest priority timer task or RTOS thread.
- `adc-sampler`: semaphore-gated ADC completion worker.
- `i2c-imu`: mutex-protected I2C sensor task.
- `telemetry`: lower-priority thread that consumes prepared samples.
