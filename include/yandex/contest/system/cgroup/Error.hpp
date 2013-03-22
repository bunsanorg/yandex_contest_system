#pragma once

#include "yandex/contest/system/Error.hpp"

#include <string>

#include <boost/filesystem/path.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    struct Error: virtual system::Error
    {
        typedef boost::error_info<struct hierarchyIdTag, std::size_t> hierarchyId;
        typedef boost::error_info<struct subsystemTag, std::string> subsystem;
        typedef boost::error_info<struct mountpointTag, boost::filesystem::path> mountpoint;
    };

    struct InconsistencyError: virtual Error {};
}}}}
