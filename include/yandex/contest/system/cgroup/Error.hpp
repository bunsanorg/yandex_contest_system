#pragma once

#include <yandex/contest/system/Error.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    struct Error: virtual system::Error
    {
        typedef boost::error_info<struct hierarchyIdTag, std::size_t> hierarchyId;
        typedef boost::error_info<struct subsystemTag, std::string> subsystem;
        typedef boost::error_info<struct mountpointTag, boost::filesystem::path> mountpoint;
        typedef boost::error_info<struct controlGroupTag, boost::filesystem::path> controlGroupPath;
    };

    struct InconsistencyError: virtual Error {};

    struct FileFormatError: virtual Error {};
    struct FileLineFormatError: virtual FileFormatError
    {
        typedef boost::error_info<struct lineTag, std::string> line;
    };
}}}}
