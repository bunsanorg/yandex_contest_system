#include <yandex/contest/system/execution/ResultError.hpp>

namespace yandex {
namespace contest {
namespace system {
namespace execution {

ResultError::ResultError(const Result &result) {
  if (result.termSig) (*this) << termSig(result.termSig.get());
  if (result.exitStatus) (*this) << exitStatus(result.exitStatus.get());
  (*this) << out(result.out);
  (*this) << err(result.err);
}

}  // namespace execution
}  // namespace system
}  // namespace contest
}  // namespace yandex
