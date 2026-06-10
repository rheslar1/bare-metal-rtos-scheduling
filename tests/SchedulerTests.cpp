#include "scheduling/Scheduler.hpp"

#include <cassert>
#include <sstream>
#include <string>
#include <vector>

namespace {

bool contains(const std::string& value, const std::string& needle) {
  return value.find(needle) != std::string::npos;
}

void bareMetalTimerSuperloopDispatchesTasks() {
  scheduling::BareMetalTimerSuperloop scheduler(scheduling::demoTasks());
  const auto report = scheduler.run(30U);

  assert(report.runnerName == "bare-metal timer superloop");
  assert(!report.events.empty());
  assert(report.timerTicks > 0U);
  assert(!report.sharedBusProtected);
  assert(report.events.front().task == "control-loop");
}

void linuxRtosUsesThreadsAndResourceGuards() {
  scheduling::LinuxThreadedRtosRunner scheduler(scheduling::demoTasks());
  const auto report = scheduler.run(30U);

  assert(report.runnerName == "linux threaded RTOS model");
  assert(!report.events.empty());
  assert(report.missedDeadlines == 0U);
  assert(report.sharedBusProtected);
  assert(report.semaphoreUsed);
  bool sawThread = false;
  bool sawMutex = false;
  bool sawSemaphore = false;
  for (const auto& event : report.events) {
    sawThread = sawThread || event.runner == "linux-thread";
    sawMutex = sawMutex || contains(event.detail, "i2c_mutex=locked");
    sawSemaphore = sawSemaphore || contains(event.detail, "adc_semaphore=used");
  }
  assert(sawThread);
  assert(sawMutex);
  assert(sawSemaphore);
}

void overloadShowsBareMetalDeadlineRisk() {
  std::vector<scheduling::TaskSpec> tasks{
      scheduling::TaskSpec{"control-loop", 5U, 4U, 1U, 0U, false, false},
      scheduling::TaskSpec{"slow-logger", 5U, 4U, 5U, 0U, false, false},
  };
  scheduling::BareMetalTimerSuperloop scheduler(tasks);
  const auto report = scheduler.run(20U);

  assert(report.missedDeadlines > 0U);
}

void reporterIncludesBothTimerAndThreadEvidence() {
  scheduling::BareMetalTimerSuperloop bareMetal(scheduling::demoTasks());
  scheduling::LinuxThreadedRtosRunner rtos(scheduling::demoTasks());
  std::ostringstream output;
  scheduling::TextScheduleReporter reporter(output);

  reporter.publish(bareMetal.run(20U));
  reporter.publish(rtos.run(20U));

  assert(contains(output.str(), "bare-metal timer superloop"));
  assert(contains(output.str(), "linux threaded RTOS model"));
  assert(contains(output.str(), "linux-thread"));
}

}  // namespace

int main() {
  bareMetalTimerSuperloopDispatchesTasks();
  linuxRtosUsesThreadsAndResourceGuards();
  overloadShowsBareMetalDeadlineRisk();
  reporterIncludesBothTimerAndThreadEvidence();
  return 0;
}
