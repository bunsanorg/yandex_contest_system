#pragma once

#include <yandex/contest/system/Error.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {

struct Error : virtual system::Error {
  using hierarchyId = boost::error_info<struct hierarchyIdTag, std::size_t>;
  using subsystem = boost::error_info<struct subsystemTag, std::string>;
  using mountpoint =
      boost::error_info<struct mountpointTag, boost::filesystem::path>;
  using controlGroupPath =
      boost::error_info<struct controlGroupTag, boost::filesystem::path>;
};

struct InconsistencyError : virtual Error {};

struct FileFormatError : virtual Error {};
struct FileLineFormatError : virtual FileFormatError {
  using line = boost::error_info<struct lineTag, std::string>;
};

}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
