#include "scheduling/Scheduler.hpp"

#include <algorithm>
#include <ostream>
#include <thread>
#include <utility>

namespace scheduling {
namespace {

DispatchStatus classify(const std::uint32_t releaseMs,
                        const std::uint32_t startMs,
                        const std::uint32_t jitterBudgetMs) {
  return (startMs > releaseMs + jitterBudgetMs) ? DispatchStatus::Late
                                                : DispatchStatus::OnTime;
}

std::string detailFor(const TaskSpec& task,
                      const DispatchStatus status,
                      const bool protectedResource) {
  std::string detail =
      task.name + " wcet=" + std::to_string(task.worstCaseExecutionMs) +
      "ms period=" + std::to_string(task.periodMs) + "ms";
  if (task.usesSharedI2cBus) {
    detail += protectedResource ? " i2c_mutex=locked" : " i2c_mutex=missing";
  }
  if (task.waitsForAdcSemaphore) {
    detail += " adc_semaphore=used";
  }
  if (status == DispatchStatus::Late) {
    detail += " deadline_jitter_exceeded";
  }
  return detail;
}

void sortEvents(std::vector<DispatchEvent>& events) {
  std::sort(events.begin(), events.end(), [](const auto& lhs, const auto& rhs) {
    if (lhs.releaseMs != rhs.releaseMs) {
      return lhs.releaseMs < rhs.releaseMs;
    }
    if (lhs.startMs != rhs.startMs) {
      return lhs.startMs < rhs.startMs;
    }
    return lhs.priority < rhs.priority;
  });
}

}  // namespace

std::string toString(const DispatchStatus status) {
  switch (status) {
    case DispatchStatus::OnTime:
      return "ON_TIME";
    case DispatchStatus::Late:
      return "LATE";
    case DispatchStatus::Blocked:
      return "BLOCKED";
  }
  return "UNKNOWN";
}

CountingSemaphore::CountingSemaphore(const std::uint32_t permits)
    : permits_(permits) {}

void CountingSemaphore::acquire() {
  std::unique_lock<std::mutex> lock(mutex_);
  condition_.wait(lock, [&] { return permits_ > 0U; });
  --permits_;
}

void CountingSemaphore::release() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    ++permits_;
  }
  condition_.notify_one();
}

std::uint32_t CountingSemaphore::permits() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return permits_;
}

BareMetalTimerSuperloop::BareMetalTimerSuperloop(std::vector<TaskSpec> tasks)
    : tasks_(std::move(tasks)) {}

ScheduleReport BareMetalTimerSuperloop::run(const std::uint32_t durationMs) const {
  ScheduleReport report;
  report.runnerName = "bare-metal timer superloop";
  report.durationMs = durationMs;
  report.sharedBusProtected = false;
  report.semaphoreUsed = false;

  std::vector<std::uint32_t> nextRelease(tasks_.size(), 0U);
  std::uint32_t now = 0U;
  while (now <= durationMs) {
    bool dispatched = false;
    ++report.timerTicks;

    for (std::size_t index = 0U; index < tasks_.size(); ++index) {
      const auto& task = tasks_[index];
      if (now < nextRelease[index]) {
        continue;
      }

      const auto release = nextRelease[index];
      const auto start = now;
      const auto finish = start + task.worstCaseExecutionMs;
      const auto status = classify(release, start, task.jitterBudgetMs);
      if (status == DispatchStatus::Late) {
        ++report.missedDeadlines;
      }

      report.events.push_back(DispatchEvent{
          release,
          start,
          finish,
          task.priority,
          task.name,
          "bare-metal-timer",
          status,
          detailFor(task, status, false)});

      nextRelease[index] += task.periodMs;
      now = finish;
      dispatched = true;
    }

    if (!dispatched) {
      ++now;
    }
  }

  return report;
}

LinuxThreadedRtosRunner::LinuxThreadedRtosRunner(std::vector<TaskSpec> tasks)
    : tasks_(std::move(tasks)) {}

ScheduleReport LinuxThreadedRtosRunner::run(const std::uint32_t durationMs) {
  ScheduleReport report;
  report.runnerName = "linux threaded RTOS model";
  report.durationMs = durationMs;
  report.timerTicks = durationMs + 1U;
  report.sharedBusProtected =
      std::any_of(tasks_.begin(), tasks_.end(), [](const auto& task) {
        return task.usesSharedI2cBus;
      });
  report.semaphoreUsed =
      std::any_of(tasks_.begin(), tasks_.end(), [](const auto& task) {
        return task.waitsForAdcSemaphore;
      });

  std::mutex eventsMutex;
  std::mutex i2cMutex;
  CountingSemaphore adcSemaphore(1U);
  std::vector<std::thread> threads;

  for (const auto& task : tasks_) {
    threads.emplace_back([&, task] {
      for (std::uint32_t release = 0U; release <= durationMs;
           release += task.periodMs) {
        if (task.waitsForAdcSemaphore) {
          adcSemaphore.acquire();
        }

        std::unique_lock<std::mutex> i2cLock(i2cMutex, std::defer_lock);
        if (task.usesSharedI2cBus) {
          i2cLock.lock();
        }

        const std::uint32_t priorityDelay =
            task.priority <= 2U ? 0U : static_cast<std::uint32_t>(task.priority - 2U);
        const std::uint32_t start = release + priorityDelay;
        const std::uint32_t finish = start + task.worstCaseExecutionMs;
        const auto status = classify(release, start, task.jitterBudgetMs);

        {
          std::lock_guard<std::mutex> eventsLock(eventsMutex);
          if (status == DispatchStatus::Late) {
            ++report.missedDeadlines;
          }
          report.events.push_back(DispatchEvent{
              release,
              start,
              finish,
              task.priority,
              task.name,
              "linux-thread",
              status,
              detailFor(task, status, true)});
        }

        if (task.usesSharedI2cBus) {
          i2cLock.unlock();
        }
        if (task.waitsForAdcSemaphore) {
          adcSemaphore.release();
        }
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  sortEvents(report.events);
  return report;
}

TextScheduleReporter::TextScheduleReporter(std::ostream& stream)
    : stream_(stream) {}

void TextScheduleReporter::publish(const ScheduleReport& report) const {
  stream_ << "runner=\"" << report.runnerName << "\" duration_ms="
          << report.durationMs << " ticks=" << report.timerTicks
          << " missed=" << report.missedDeadlines
          << " bus_protected=" << (report.sharedBusProtected ? "yes" : "no")
          << " semaphore=" << (report.semaphoreUsed ? "yes" : "no") << '\n';
  for (const auto& event : report.events) {
    stream_ << "  [" << toString(event.status) << "] " << event.runner << " "
            << event.task << " release=" << event.releaseMs
            << " start=" << event.startMs << " finish=" << event.finishMs
            << " priority=" << static_cast<unsigned int>(event.priority)
            << " detail=\"" << event.detail << "\"\n";
  }
}

std::vector<TaskSpec> demoTasks() {
  return {
      TaskSpec{"control-loop", 5U, 1U, 1U, 0U, false, false},
      TaskSpec{"adc-sampler", 10U, 1U, 2U, 1U, false, true},
      TaskSpec{"i2c-imu", 20U, 2U, 3U, 2U, true, false},
      TaskSpec{"telemetry", 50U, 3U, 5U, 5U, true, false},
  };
}

}  // namespace scheduling
