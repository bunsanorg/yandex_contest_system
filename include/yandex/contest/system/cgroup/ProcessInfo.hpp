#pragma once

#include <yandex/contest/system/cgroup/Error.hpp>
#include <yandex/contest/system/cgroup/ProcessHierarchyInfo.hpp>

#include <boost/iterator/transform_iterator.hpp>

#include <functional>
#include <iostream>
#include <unordered_map>

#include <sys/types.h>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {

struct ProcessInfoError : virtual Error {};
struct ProcessInfoInconsistencyError : virtual ProcessInfoError,
                                       virtual InconsistencyError {};
struct ProcessInfoDuplicateHierarchiesError : virtual ProcessInfoError,
                                              virtual FileFormatError {};

class ProcessInfo {
 private:
  using id2controlGroupType =
      std::unordered_map<std::size_t, boost::filesystem::path>;
  using map_const_iterator = id2controlGroupType::const_iterator;
  using map_value_type = id2controlGroupType::value_type;

 public:
  using value_type = ProcessHierarchyInfo;

 private:
  struct IteratorConverter
      : std::unary_function<const map_value_type &, value_type> {
    value_type operator()(const map_value_type &value) const;
  };

 public:
  using const_iterator =
      boost::transform_iterator<IteratorConverter, map_const_iterator>;

 public:
  ProcessInfo() = default;
  ProcessInfo(const ProcessInfo &) = default;
  ProcessInfo(ProcessInfo &&) = default;
  ProcessInfo &operator=(const ProcessInfo &) = default;
  ProcessInfo &operator=(ProcessInfo &&) = default;

  void swap(ProcessInfo &processInfo) noexcept;

  ProcessHierarchyInfo byHierarchyId(std::size_t hierarchyId) const;
  ProcessHierarchyInfo bySubsystem(const std::string &subsystem) const;
  ProcessHierarchyInfo byMountpoint(
      const boost::filesystem::path &mountpoint) const;

  const_iterator begin() const;
  const_iterator cbegin() const;
  const_iterator end() const;
  const_iterator cend() const;

 public:
  static ProcessInfo fromFile(const boost::filesystem::path &path);
  static ProcessInfo forPid(pid_t pid);
  static ProcessInfo forSelf();

 private:
  ProcessHierarchyInfo getProcessHierarchyInfo(const HierarchyInfo &info) const;

 private:
  id2controlGroupType id2controlGroup_;
};

inline void swap(ProcessInfo &a, ProcessInfo &b) noexcept { a.swap(b); }

inline ProcessInfo::const_iterator begin(const ProcessInfo &processInfo) {
  return processInfo.cbegin();
}

inline ProcessInfo::const_iterator end(const ProcessInfo &processInfo) {
  return processInfo.cend();
}

std::ostream &operator<<(std::ostream &out, const ProcessInfo &processInfo);

}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
