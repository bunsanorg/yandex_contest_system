#include <yandex/contest/system/cgroup/HierarchyInfo.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    std::ostream &operator<<(std::ostream &out, const HierarchyInfo &hierarchyInfo)
    {
        out << "{ id = " << hierarchyInfo.id << ", subsystems = {";
        {
            bool first = true;
            for (const std::string &subsystem: hierarchyInfo.subsystems)
            {
                if (!first)
                    out << ", ";
                first = false;
                out << '"' << subsystem << '"';
            }
        }
        out << "}, ";
        if (hierarchyInfo.mountpoint)
            out << "mountpoint = " << hierarchyInfo.mountpoint.get();
        else
            out << "not mounted";
        out << " }";
        return out;
    }
}}}}
