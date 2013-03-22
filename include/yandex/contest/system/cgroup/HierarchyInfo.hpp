#pragma once

#include <iostream>
#include <string>
#include <unordered_set>

#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    struct HierarchyInfo
    {
        std::size_t id = 0;
        std::unordered_set<std::string> subsystems;
        boost::optional<boost::filesystem::path> mountpoint;
    };

    std::ostream &operator<<(std::ostream &out, const HierarchyInfo &hierarchyInfo);
}}}}
