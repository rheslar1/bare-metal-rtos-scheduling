#include "scheduling/Scheduler.hpp"

#include <iostream>

int main() {
  const auto tasks = scheduling::demoTasks();
  scheduling::BareMetalTimerSuperloop bareMetal(tasks);
  scheduling::LinuxThreadedRtosRunner linuxRtos(tasks);
  scheduling::TextScheduleReporter reporter(std::cout);

  std::cout << "Bare-Metal RTOS Scheduling\n";
  std::cout << "Bare-metal version: timer ISR releases cooperative superloop work\n";
  std::cout << "RTOS/Linux version: timer-released tasks run on Linux threads with mutex/semaphore guards\n\n";

  const auto bareMetalReport = bareMetal.run(30U);
  reporter.publish(bareMetalReport);
  std::cout << '\n';

  const auto linuxReport = linuxRtos.run(30U);
  reporter.publish(linuxReport);

  return linuxReport.missedDeadlines == 0U ? 0 : 1;
}
