#include "yandex/contest/system/cgroup/ProcessInfo.hpp"
#include "yandex/contest/system/cgroup/ProcPidCgroup.hpp"
#include "yandex/contest/system/cgroup/SystemInfo.hpp"

#include <boost/format.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    ProcessInfo::value_type ProcessInfo::IteratorConverter::operator()(const map_value_type &value) const
    {
        ProcessHierarchyInfo processHierarchyInfo = {
            .hierarchy = SystemInfo::instance()->byHierarchyId(value.first),
            .controlGroup = value.second,
        };
        return processHierarchyInfo;
    }

    void ProcessInfo::swap(ProcessInfo &processInfo) noexcept
    {
        id2controlGroup_.swap(processInfo.id2controlGroup_);
    }

    ProcessHierarchyInfo ProcessInfo::byHierarchyId(const std::size_t hierarchyId) const
    {
        return getProcessHierarchyInfo(SystemInfo::instance()->byHierarchyId(hierarchyId));
    }

    ProcessHierarchyInfo ProcessInfo::bySubsystem(const std::string &subsystem) const
    {
        return getProcessHierarchyInfo(SystemInfo::instance()->bySubsystem(subsystem));
    }

    ProcessHierarchyInfo ProcessInfo::byMountpoint(const boost::filesystem::path &mountpoint) const
    {
        return getProcessHierarchyInfo(SystemInfo::instance()->byMountpoint(mountpoint));
    }

    ProcessInfo::const_iterator ProcessInfo::begin() const
    {
        return cbegin();
    }

    ProcessInfo::const_iterator ProcessInfo::cbegin() const
    {
        return const_iterator(id2controlGroup_.cbegin());
    }

    ProcessInfo::const_iterator ProcessInfo::end() const
    {
        return cend();
    }

    ProcessInfo::const_iterator ProcessInfo::cend() const
    {
        return const_iterator(id2controlGroup_.cend());
    }

    ProcessHierarchyInfo ProcessInfo::getProcessHierarchyInfo(const HierarchyInfo &hierarchyInfo) const
    {
        const auto iter = id2controlGroup_.find(hierarchyInfo.id);
        if (iter == id2controlGroup_.end())
            BOOST_THROW_EXCEPTION(ProcessInfoInconsistencyError() <<
                                  ProcessInfoInconsistencyError::message(
                                      "Unable to find cgroup for hierarchy's id.") <<
                                  ProcessInfoInconsistencyError::hierarchyId(hierarchyInfo.id));
        ProcessHierarchyInfo processHierarchyInfo = {
            .hierarchy = hierarchyInfo,
            .controlGroup = iter->second,
        };
        return processHierarchyInfo;
    }

    ProcessInfo ProcessInfo::fromFile(const boost::filesystem::path &path)
    {
        ProcessInfo processInfo;
        ProcPidCgroup cgroup;
        cgroup.load(path);
        for (const ProcPidCgroup::Entry &entry: cgroup)
        {
            if (processInfo.id2controlGroup_.find(entry.hierarchyId) != processInfo.id2controlGroup_.end())
                BOOST_THROW_EXCEPTION(ProcessInfoDuplicateHierarchiesError() <<
                                      ProcessInfoDuplicateHierarchiesError::path(path) <<
                                      ProcessInfoDuplicateHierarchiesError::hierarchyId(entry.hierarchyId));
            processInfo.id2controlGroup_[entry.hierarchyId] = entry.controlGroup;
            // TODO Should we check subsystems field for consistency with SystemInfo::instance()?
        }
        return processInfo;
    }

    ProcessInfo ProcessInfo::forPid(const pid_t pid)
    {
        return fromFile(str(boost::format("/proc/%1%/cgroup") % pid));
    }

    ProcessInfo ProcessInfo::forSelf()
    {
        return fromFile("/proc/self/cgroup");
    }

    std::ostream &operator<<(std::ostream &out, const ProcessInfo &processInfo)
    {
        out << "{ ";
        bool first = true;
        for (const ProcessHierarchyInfo &processHierarchyInfo: processInfo)
        {
            if (!first)
                out << ", ";
            first = false;
            out << processHierarchyInfo;
        }
        out << " }";
        return out;
    }
}}}}
