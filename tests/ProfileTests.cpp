#include <array>
#include <cassert>
#include <string_view>

class IReadinessRule {
 public:
  virtual ~IReadinessRule() = default;
  virtual bool passes(std::string_view evidenceTarget) const = 0;
};

class RequiredEvidenceRule final : public IReadinessRule {
 public:
  bool passes(std::string_view evidenceTarget) const override {
    return !evidenceTarget.empty();
  }
};

struct ProjectProfile {
  std::string_view title;
  std::string_view summary;
  std::string_view evidenceTarget;
  std::array<std::string_view, 9> tags;
};

constexpr ProjectProfile profile{
  "Bare-Metal RTOS Scheduling",
  "Multi-threaded FreeRTOS or Zephyr application with queues, mutexes, semaphores, interrupt handoff, and priority inversion notes.",
  "Real concurrency, deterministic task design, shared-resource safety, and explainable scheduler tradeoffs.",
  {
    "C++17",
    "C++ Design Patterns",
    "SOLID",
    "FreeRTOS",
    "Zephyr",
    "IPC",
    "Mutexes",
    "Semaphores",
    "Priority inheritance"
  }
};

int main() {
  const RequiredEvidenceRule rule;
  assert(!profile.title.empty());
  assert(!profile.summary.empty());
  assert(rule.passes(profile.evidenceTarget));
  assert(profile.tags[0] == "C++17");
  return 0;
}
