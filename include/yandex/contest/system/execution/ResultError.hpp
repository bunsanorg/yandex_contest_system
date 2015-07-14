#pragma once

#include <yandex/contest/system/Error.hpp>

#include <yandex/contest/system/execution/Result.hpp>

namespace yandex {
namespace contest {
namespace system {
namespace execution {

struct ResultError : virtual Error {
  ResultError() = default;
  explicit ResultError(const Result &result);

  using termSig = boost::error_info<struct termSigtag, int>;
  using exitStatus = boost::error_info<struct exitStatusTag, int>;
  using out = boost::error_info<struct outTag, std::string>;
  using err = boost::error_info<struct errTag, std::string>;
};

}  // namespace execution
}  // namespace system
}  // namespace contest
}  // namespace yandex
