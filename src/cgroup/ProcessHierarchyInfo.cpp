#include <yandex/contest/system/cgroup/ProcessHierarchyInfo.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    std::ostream &operator<<(std::ostream &out,
                             const ProcessHierarchyInfo &processHierarchyInfo)
    {
        out << "{ hierarchy = " << processHierarchyInfo.hierarchy << ", ";
        out << "cgroup = " << processHierarchyInfo.controlGroup << " }";
        return out;
    }
}}}}
