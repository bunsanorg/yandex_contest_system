#pragma once

#include <yandex/contest/system/cgroup/SubsystemBase.hpp>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {

class CpuSetBase : virtual public SubsystemBase<CpuSetBase> {
 public:
  static const std::string SUBSYSTEM_NAME;

 public:
  std::string cpus() const;
  void setCpus(const std::string &cpus) const;

  std::string mems() const;
  void setMems(const std::string &mems) const;

  bool memoryMigrate() const;
  void setMemoryMigrate(bool memoryMigrate = true) const;
};

using CpuSet = Subsystem<CpuSetBase>;

}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
