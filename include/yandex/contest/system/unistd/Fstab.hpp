#pragma once

#include <yandex/contest/system/unistd/MountEntry.hpp>

#include <boost/filesystem/path.hpp>

#include <vector>

namespace yandex {
namespace contest {
namespace system {
namespace unistd {

struct Fstab {
  std::vector<MountEntry> entries;

  /// Load from file with fstab(5) syntax.
  void load(const boost::filesystem::path &fstab);

  /// Save to file with fstab(5) syntax.
  void save(const boost::filesystem::path &fstab) const;
};

inline std::vector<MountEntry>::iterator begin(Fstab &fstab) {
  return begin(fstab.entries);
}

inline std::vector<MountEntry>::const_iterator begin(const Fstab &fstab) {
  return begin(fstab.entries);
}

inline std::vector<MountEntry>::iterator end(Fstab &fstab) {
  return end(fstab.entries);
}

inline std::vector<MountEntry>::const_iterator end(const Fstab &fstab) {
  return end(fstab.entries);
}

}  // namespace unistd
}  // namespace system
}  // namespace contest
}  // namespace yandex
