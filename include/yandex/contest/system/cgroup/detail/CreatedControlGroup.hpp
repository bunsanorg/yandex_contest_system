#pragma once

#include <yandex/contest/system/cgroup/SingleControlGroup.hpp>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {
namespace detail {

class CreatedControlGroup : public SingleControlGroup {
 public:
  CreatedControlGroup(const SystemInfoPointer &systemInfo,
                      std::size_t hierarchyId,
                      const boost::filesystem::path &controlGroup, mode_t mode,
                      const SingleControlGroupPointer &parent);

  ~CreatedControlGroup() override;

 protected:
  void printSingle(std::ostream &out) const override;
};

}  // namespace detail
}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
