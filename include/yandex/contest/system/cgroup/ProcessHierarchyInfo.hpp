#pragma once

#include <yandex/contest/system/cgroup/HierarchyInfo.hpp>

#include <iostream>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    struct ProcessHierarchyInfo
    {
        HierarchyInfo hierarchy;
        boost::filesystem::path controlGroup;
    };

    std::ostream &operator<<(std::ostream &out, const ProcessHierarchyInfo &processHierarchyInfo);
}}}}
