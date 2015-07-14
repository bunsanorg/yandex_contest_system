#pragma once

#include <yandex/contest/system/unistd/ProcessResult.hpp>

#include <string>

namespace yandex {
namespace contest {
namespace system {
namespace execution {

struct Result : unistd::ProcessResult {
  using unistd::ProcessResult::ProcessResult;

  Result() = default;
  Result(const Result &) = default;
  Result &operator=(const Result &) = default;

  std::string out;
  std::string err;
};

}  // namespace execution
}  // namespace system
}  // namespace contest
}  // namespace yandex
