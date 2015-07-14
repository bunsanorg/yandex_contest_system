#pragma once

#include <yandex/contest/system/cgroup/SingleControlGroup.hpp>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {
namespace detail {

class AttachedControlGroup : public SingleControlGroup {
 public:
  AttachedControlGroup(const SystemInfoPointer &systemInfo,
                       std::size_t hierarchyId,
                       const boost::filesystem::path &controlGroup,
                       const SingleControlGroupPointer &parent);

 protected:
  void printSingle(std::ostream &out) const override;
};

}  // namespace detail
}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
