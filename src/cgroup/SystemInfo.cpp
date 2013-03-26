#include "yandex/contest/system/cgroup/SystemInfo.hpp"
#include "yandex/contest/system/cgroup/ProcPidCgroup.hpp"
#include "yandex/contest/system/unistd/Fstab.hpp"

#include "bunsan/enable_error_info.hpp"
#include "bunsan/filesystem/fstream.hpp"

#include <boost/assert.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

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
                BOOST_THROW_EXCEPTION(SystemInfoDuplicateHierarchiesError() <<
                                      SystemInfoDuplicateHierarchiesError::hierarchyId(entry.hierarchyId));
            HierarchyInfo &info = id2hierarchy_[entry.hierarchyId];
            info.id = std::move(entry.hierarchyId);
            info.subsystems = std::move(entry.subsystems);
            for (const std::string &subsystem: info.subsystems)
            {
                if (subsystem2id_.find(subsystem) != subsystem2id_.end())
                    BOOST_THROW_EXCEPTION(SystemInfoDuplicateSubsystemsError() <<
                                          SystemInfoDuplicateSubsystemsError::hierarchyId(info.id) <<
                                          SystemInfoDuplicateSubsystemsError::subsystem(subsystem));
                subsystem2id_[subsystem] = info.id;
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
                    boost::algorithm::split(opts, entry.opts, boost::algorithm::is_any_of(","));
                    for (const std::string &opt: opts)
                    {
                        static const std::unordered_set<std::string> knownSubsystems = {
                            "blkio",
                            "cpu",
                            "cpuacct",
                            "cpuset",
                            "devices",
                            "freezer",
                            "memory",
                            "net_cls",
                            "net_prio",
                            "ns",
                            "perf_event",
                        };
                        if (boost::algorithm::starts_with(opt, "name=") ||
                            knownSubsystems.find(opt) != knownSubsystems.end())
                        {
                            subsystems.insert(opt);
                        }
                    }
                    if (subsystems.empty())
                        BOOST_THROW_EXCEPTION(SystemInfoNoSubsystemsError());
                    const std::string subsystem = *subsystems.begin();
                    const auto iter = subsystem2id_.find(subsystem);
                    if (iter == subsystem2id_.end())
                        BOOST_THROW_EXCEPTION(SystemInfoInconsistencyError() <<
                                              SystemInfoInconsistencyError::message(
                                                  "/proc/mounts is not consistent with /proc/self/cgroup"));
                    const auto iter2 = id2hierarchy_.find(iter->second);
                    BOOST_ASSERT(iter2 != id2hierarchy_.end());
                    HierarchyInfo &info = iter2->second;
                    if (subsystems != info.subsystems)
                        BOOST_THROW_EXCEPTION(SystemInfoInconsistencyError() <<
                                              SystemInfoInconsistencyError::message(
                                                  "subsystems from /proc/mounts are not equal "
                                                  "to subsystems from /proc/self/cgroup"));
                    info.mountpoint = entry.dir;
                    mountpoint2id_[entry.dir] = info.id;
                }
            }
            BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(
                bunsan::filesystem::error::path("/proc/self/cgroup") <<
                SystemInfoProcMountsFormatError::mountEntry(entry))
        }
    }

    const HierarchyInfo &SystemInfo::byHierarchyId(const std::size_t hierarchyId) const
    {
        const auto iter = id2hierarchy_.find(hierarchyId);
        if (iter == id2hierarchy_.end())
            BOOST_THROW_EXCEPTION(SystemInfoUnknownHierarchyError() <<
                                  SystemInfoUnknownHierarchyError::hierarchyId(hierarchyId));
        return iter->second;
    }

    const HierarchyInfo &SystemInfo::bySubsystem(const std::string &subsystem) const
    {
        const auto iter = subsystem2id_.find(subsystem);
        if (iter == subsystem2id_.end())
            BOOST_THROW_EXCEPTION(SystemInfoUnknownHierarchyError() <<
                                  SystemInfoUnknownHierarchyError::subsystem(subsystem));
        return byIdNoFail(iter->second);
    }

    const HierarchyInfo &SystemInfo::byMountpoint(const boost::filesystem::path &mountpoint) const
    {
        const auto iter = mountpoint2id_.find(mountpoint);
        if (iter == mountpoint2id_.end())
            BOOST_THROW_EXCEPTION(SystemInfoUnknownHierarchyError() <<
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
        SystemInfoPointer info = atomic_load(&instance_);
        if (forceUpdate || !info)
        {
            info.reset(new SystemInfo);
            atomic_store(&instance_, info);
        }
        return info;
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
