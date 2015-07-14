#pragma once

#include <yandex/contest/system/cgroup/Error.hpp>
#include <yandex/contest/system/cgroup/HierarchyInfo.hpp>
#include <yandex/contest/system/unistd/MountEntry.hpp>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include <functional>
#include <iostream>
#include <unordered_map>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {

struct SystemInfoError : virtual Error {};
struct SystemInfoInconsistencyError : virtual SystemInfoError,
                                      virtual InconsistencyError {};
struct SystemInfoDuplicateHierarchiesError
    : virtual SystemInfoInconsistencyError {};
struct SystemInfoDuplicateSubsystemsError
    : virtual SystemInfoInconsistencyError {};

struct SystemInfoProcMountsFormatError : virtual SystemInfoError,
                                         virtual FileFormatError {
  using mountEntry =
      boost::error_info<struct mountEntryTag, unistd::MountEntry>;
};

struct SystemInfoNoSubsystemsError : virtual SystemInfoProcMountsFormatError {};

struct SystemInfoUnknownHierarchyError : virtual SystemInfoError {};

class SystemInfo : private boost::noncopyable {
 private:
  using id2hierarchy_type = std::unordered_map<std::size_t, HierarchyInfo>;
  using map_const_iterator = id2hierarchy_type::const_iterator;
  using map_value_type = id2hierarchy_type::value_type;

 public:
  using SystemInfoPointer = boost::shared_ptr<SystemInfo>;
  using value_type = map_value_type::second_type;

 private:
  struct IteratorConverter
      : std::unary_function<const map_value_type &, const value_type &> {
    const value_type &operator()(const map_value_type &value) const {
      return value.second;
    }
  };

 public:
  using const_iterator =
      boost::transform_iterator<IteratorConverter, map_const_iterator>;

 public:
  const HierarchyInfo &byHierarchyId(std::size_t hierarchyId) const;
  const HierarchyInfo &bySubsystem(const std::string &subsystem) const;
  const HierarchyInfo &byMountpoint(
      const boost::filesystem::path &mountpoint) const;

  const_iterator begin() const;
  const_iterator cbegin() const;
  const_iterator end() const;
  const_iterator cend() const;

 public:
  static SystemInfoPointer instance(bool forceUpdate = false);

 private:
  SystemInfo();

  void loadHierarchies();
  void loadMountpoints();

  const HierarchyInfo &byIdNoFail(std::size_t id) const;

 private:
  id2hierarchy_type id2hierarchy_;
  boost::unordered_map<boost::filesystem::path, std::size_t> mountpoint2id_;
  std::unordered_map<std::string, std::size_t> subsystem2id_;
};

using SystemInfoPointer = SystemInfo::SystemInfoPointer;

inline SystemInfo::const_iterator begin(const SystemInfo &systemInfo) {
  return systemInfo.cbegin();
}

inline SystemInfo::const_iterator end(const SystemInfo &systemInfo) {
  return systemInfo.cend();
}

std::ostream &operator<<(std::ostream &out, const SystemInfo &systemInfo);

}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
