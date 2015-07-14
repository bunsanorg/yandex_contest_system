#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

#include <ostream>
#include <string>
#include <unordered_set>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {

struct HierarchyInfo {
  std::size_t id = 0;
  std::unordered_set<std::string> subsystems;
  boost::optional<boost::filesystem::path> mountpoint;
};

std::ostream &operator<<(std::ostream &out, const HierarchyInfo &hierarchyInfo);

}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
