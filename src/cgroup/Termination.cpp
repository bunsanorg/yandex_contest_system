#include <yandex/contest/system/cgroup/Termination.hpp>

#include <yandex/contest/system/cgroup/Freezer.hpp>
#include <yandex/contest/system/unistd/Operations.hpp>

#include <yandex/contest/StreamLog.hpp>

#include <boost/scope_exit.hpp>

#include <thread>

#include <signal.h>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {

static std::size_t terminate_(const ControlGroupPointer &controlGroup) {
  STREAM_TRACE << "Attempt to terminate all tasks in " << controlGroup;
  Freezer freezer(controlGroup);
  BOOST_SCOPE_EXIT_ALL(&freezer) { freezer.unfreeze(); };
  freezer.freeze();
  std::size_t terminated = 0;
  for (const pid_t pid : controlGroup->tasks()) {
    STREAM_TRACE << "Terminating task { pid = " << pid << " } "
                 << "in " << *controlGroup;
    unistd::kill(pid, SIGKILL);
    ++terminated;
  }
  STREAM_DEBUG << "Terminated " << terminated << " tasks "
               << "in " << *controlGroup;
  return terminated;
}

static void waitEmpty_(const ControlGroupPointer &controlGroup) {
  STREAM_TRACE << "Waiting for " << *controlGroup << " to become empty...";
  // Manual calibrated numbers.
  for (std::chrono::milliseconds duration{10};;) {
    if (controlGroup->tasks().size() > 0) {
      std::this_thread::sleep_for(duration);
    } else {
      break;
    }
    if (duration < std::chrono::milliseconds(100))
      duration += std::chrono::milliseconds(5);
  }
}

std::size_t terminate(const ControlGroupPointer &controlGroup) {
  const std::size_t terminated = terminate_(controlGroup);
  waitEmpty_(controlGroup);
  return terminated;
}

TerminationGuard::TerminationGuard(const ControlGroupPointer &controlGroup)
    : controlGroup_(controlGroup) {}

TerminationGuard::operator bool() const {
  return static_cast<bool>(controlGroup_);
}

void TerminationGuard::detach() { controlGroup_ = nullptr; }

void TerminationGuard::terminate() { cgroup::terminate(controlGroup_); }

TerminationGuard::~TerminationGuard() {
  if (controlGroup_) {
    try {
      terminate();
    } catch (std::exception &e) {
      STREAM_ERROR << "Unable to terminate tasks "
                   << "in " << *controlGroup_ << " due to: " << e.what();
    } catch (...) {
      STREAM_ERROR << "Unable to terminate tasks "
                   << "in " << *controlGroup_ << " due to unknown exception";
    }
  }
}

}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
