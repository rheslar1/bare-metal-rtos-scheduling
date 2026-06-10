#ifndef SCHEDULING_SCHEDULER_HPP_
#define SCHEDULING_SCHEDULER_HPP_

#include <condition_variable>
#include <cstdint>
#include <iosfwd>
#include <mutex>
#include <string>
#include <vector>

namespace scheduling {

enum class DispatchStatus {
  OnTime,
  Late,
  Blocked
};

std::string toString(DispatchStatus status);

struct TaskSpec {
  std::string name;
  std::uint32_t periodMs{};
  std::uint32_t worstCaseExecutionMs{};
  std::uint8_t priority{};
  std::uint32_t jitterBudgetMs{};
  bool usesSharedI2cBus{};
  bool waitsForAdcSemaphore{};
};

struct DispatchEvent {
  std::uint32_t releaseMs{};
  std::uint32_t startMs{};
  std::uint32_t finishMs{};
  std::uint8_t priority{};
  std::string task;
  std::string runner;
  DispatchStatus status{DispatchStatus::Late};
  std::string detail;
};

struct ScheduleReport {
  std::string runnerName;
  std::uint32_t durationMs{};
  std::uint32_t timerTicks{};
  std::uint32_t missedDeadlines{};
  bool sharedBusProtected{};
  bool semaphoreUsed{};
  std::vector<DispatchEvent> events;
};

class CountingSemaphore {
 public:
  explicit CountingSemaphore(std::uint32_t permits);

  void acquire();
  void release();
  std::uint32_t permits() const;

 private:
  mutable std::mutex mutex_;
  std::condition_variable condition_;
  std::uint32_t permits_{};
};

class BareMetalTimerSuperloop {
 public:
  explicit BareMetalTimerSuperloop(std::vector<TaskSpec> tasks);

  ScheduleReport run(std::uint32_t durationMs) const;

 private:
  std::vector<TaskSpec> tasks_;
};

class LinuxThreadedRtosRunner {
 public:
  explicit LinuxThreadedRtosRunner(std::vector<TaskSpec> tasks);

  ScheduleReport run(std::uint32_t durationMs);

 private:
  std::vector<TaskSpec> tasks_;
};

class TextScheduleReporter {
 public:
  explicit TextScheduleReporter(std::ostream& stream);

  void publish(const ScheduleReport& report) const;

 private:
  std::ostream& stream_;
};

std::vector<TaskSpec> demoTasks();

}  // namespace scheduling

#endif  // SCHEDULING_SCHEDULER_HPP_
