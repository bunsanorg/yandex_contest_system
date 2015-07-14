#pragma once

#include <yandex/contest/system/cgroup/Error.hpp>

#include <boost/filesystem/path.hpp>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {

struct ControlGroupIsNotMountedError : virtual Error {};

/*!
 * \brief Returns the first mounted cgroup
 * or throws ControlGroupIsNotMountedError.
 *
 * \return First mounted cgroup.
 */
boost::filesystem::path getMountPoint();

}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
