#pragma once

#include <yandex/contest/system/cgroup/ControlGroup.hpp>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {

/*!
 * \brief Terminates all tasks in specified control group.
 *
 * \return Number of terminated tasks.
 *
 * \note Requires Freezer subsystem to be available.
 * \note Blocks until all tasks are terminated and control group
 * can be removed if no child control groups are created.
 */
std::size_t terminate(const ControlGroupPointer &controlGroup);

class TerminationGuard {
 public:
  TerminationGuard() = default;

  TerminationGuard(TerminationGuard &&) = default;
  TerminationGuard &operator=(TerminationGuard &&) = default;

  TerminationGuard(const TerminationGuard &) = delete;
  TerminationGuard &operator=(const TerminationGuard &) = delete;

  explicit TerminationGuard(const ControlGroupPointer &controlGroup);

  explicit operator bool() const;

  void detach();
  void terminate();

  ~TerminationGuard();

 private:
  ControlGroupPointer controlGroup_;
};

}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
