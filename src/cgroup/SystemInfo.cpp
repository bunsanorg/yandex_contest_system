#include <yandex/contest/system/cgroup/SystemInfo.hpp>

#include <yandex/contest/system/cgroup/ProcPidCgroup.hpp>
#include <yandex/contest/system/unistd/Fstab.hpp>

#include <bunsan/enable_error_info.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/assert.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    SystemInfo::SystemInfo()
    {
        loadHierarchies();
        loadMountpoints();
    }

    void SystemInfo::loadHierarchies()
    {
        ProcPidCgroup cgroup;
        cgroup.load("/proc/self/cgroup");
        for (ProcPidCgroup::Entry &entry: cgroup)
        {
            if (id2hierarchy_.find(entry.hierarchyId) != id2hierarchy_.end())
                BOOST_THROW_EXCEPTION(
                    SystemInfoDuplicateHierarchiesError() <<
                    SystemInfoDuplicateHierarchiesError::hierarchyId(
                        entry.hierarchyId));
            HierarchyInfo &hierarchyInfo = id2hierarchy_[entry.hierarchyId];
            hierarchyInfo.id = std::move(entry.hierarchyId);
            hierarchyInfo.subsystems = std::move(entry.subsystems);
            for (const std::string &subsystem: hierarchyInfo.subsystems)
            {
                if (subsystem2id_.find(subsystem) != subsystem2id_.end())
                    BOOST_THROW_EXCEPTION(
                        SystemInfoDuplicateSubsystemsError() <<
                        SystemInfoDuplicateSubsystemsError::hierarchyId(
                            hierarchyInfo.id) <<
                        SystemInfoDuplicateSubsystemsError::subsystem(subsystem));
                subsystem2id_[subsystem] = hierarchyInfo.id;
            }
        }
    }

    void SystemInfo::loadMountpoints()
    {
        unistd::Fstab fstab;
        fstab.load("/proc/mounts");
        for (const unistd::MountEntry &entry: fstab)
        {
            BUNSAN_EXCEPTIONS_WRAP_BEGIN()
            {
                if (entry.type == "cgroup")
                {
                    std::unordered_set<std::string> subsystems;
                    std::vector<std::string> opts;
                    boost::algorithm::split(
                        opts,
                        entry.opts,
                        boost::algorithm::is_any_of(",")
                    );
                    for (const std::string &opt: opts)
                    {
                        // Every process belongs to control group
                        // if it is mounted. That means subsystem2id_
                        // knows all subsystems mounted in system
                        // and we can use it as filter.
                        if (subsystem2id_.find(opt) != subsystem2id_.end())
                            subsystems.insert(opt);
                    }
                    if (subsystems.empty())
                        BOOST_THROW_EXCEPTION(SystemInfoNoSubsystemsError());
                    const std::string subsystem = *subsystems.begin();
                    const auto iter = subsystem2id_.find(subsystem);
                    if (iter == subsystem2id_.end())
                        BOOST_THROW_EXCEPTION(
                            SystemInfoInconsistencyError() <<
                            SystemInfoInconsistencyError::message(
                                "/proc/mounts is not consistent "
                                "with /proc/self/cgroup"));
                    const auto iter2 = id2hierarchy_.find(iter->second);
                    BOOST_ASSERT(iter2 != id2hierarchy_.end());
                    HierarchyInfo &hierarchyInfo = iter2->second;
                    if (subsystems != hierarchyInfo.subsystems)
                        BOOST_THROW_EXCEPTION(
                            SystemInfoInconsistencyError() <<
                            SystemInfoInconsistencyError::message(
                                "subsystems from /proc/mounts are not equal "
                                "to subsystems from /proc/self/cgroup"));
                    hierarchyInfo.mountpoint = entry.dir;
                    mountpoint2id_[entry.dir] = hierarchyInfo.id;
                }
            }
            BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(
                bunsan::filesystem::error::path("/proc/self/cgroup") <<
                SystemInfoProcMountsFormatError::mountEntry(entry))
        }
    }

    const HierarchyInfo &SystemInfo::byHierarchyId(
        const std::size_t hierarchyId) const
    {
        const auto iter = id2hierarchy_.find(hierarchyId);
        if (iter == id2hierarchy_.end())
            BOOST_THROW_EXCEPTION(
                SystemInfoUnknownHierarchyError() <<
                SystemInfoUnknownHierarchyError::hierarchyId(hierarchyId));
        return iter->second;
    }

    const HierarchyInfo &SystemInfo::bySubsystem(
        const std::string &subsystem) const
    {
        const auto iter = subsystem2id_.find(subsystem);
        if (iter == subsystem2id_.end())
            BOOST_THROW_EXCEPTION(
                SystemInfoUnknownHierarchyError() <<
                SystemInfoUnknownHierarchyError::subsystem(subsystem));
        return byIdNoFail(iter->second);
    }

    const HierarchyInfo &SystemInfo::byMountpoint(
        const boost::filesystem::path &mountpoint) const
    {
        const auto iter = mountpoint2id_.find(mountpoint);
        if (iter == mountpoint2id_.end())
            BOOST_THROW_EXCEPTION(
                SystemInfoUnknownHierarchyError() <<
                SystemInfoUnknownHierarchyError::mountpoint(mountpoint));
        return byIdNoFail(iter->second);
    }

    const HierarchyInfo &SystemInfo::byIdNoFail(const std::size_t id) const
    {
        const auto iter = id2hierarchy_.find(id);
        BOOST_ASSERT(iter != id2hierarchy_.end());
        return iter->second;
    }

    SystemInfo::const_iterator SystemInfo::begin() const
    {
        return cbegin();
    }

    SystemInfo::const_iterator SystemInfo::cbegin() const
    {
        return const_iterator(id2hierarchy_.cbegin());
    }

    SystemInfo::const_iterator SystemInfo::end() const
    {
        return cend();
    }

    SystemInfo::const_iterator SystemInfo::cend() const
    {
        return const_iterator(id2hierarchy_.cend());
    }

    SystemInfoPointer SystemInfo::instance(const bool forceUpdate)
    {
        static SystemInfoPointer instance_;
        SystemInfoPointer systemInfo = atomic_load(&instance_);
        if (forceUpdate || !systemInfo)
        {
            systemInfo.reset(new SystemInfo);
            atomic_store(&instance_, systemInfo);
        }
        return systemInfo;
    }

    std::ostream &operator<<(std::ostream &out, const SystemInfo &systemInfo)
    {
        out << "{ ";
        bool first = true;
        for (const HierarchyInfo &hierarchyInfo: systemInfo)
        {
            if (!first)
                out << ", ";
            first = false;
            out << hierarchyInfo;
        }
        out << " }";
        return out;
    }
}}}}
