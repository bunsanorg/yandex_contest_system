#pragma once

#include <cstdint>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {

using Count = std::uint64_t;

namespace detail {
template <typename Units>
struct UnitsConverter {
  static Count unitsToCount(const Units units) {
    return static_cast<Count>(units);
  }

  static Units countToUnits(const Count count) {
    return static_cast<Units>(count);
  }
};
}  // namespace detail

}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
