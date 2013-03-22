#include "yandex/contest/system/cgroup/SystemInfo.hpp"
#include "yandex/contest/system/unistd/Fstab.hpp"

#include "bunsan/enable_error_info.hpp"
#include "bunsan/filesystem/fstream.hpp"

#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    SystemInfo::SystemInfo()
    {
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            bunsan::filesystem::ifstream fin("/proc/self/cgroup");
            std::string line;
            while (std::getline(fin, line))
            {
                const std::size_t pos1 = line.find(':');
                BOOST_ASSERT(pos1 != std::string::npos);
                const std::size_t pos2 = line.find(':', pos1 + 1);
                BOOST_ASSERT(pos2 != std::string::npos);
                BOOST_ASSERT(pos1 < pos2);
                const std::size_t hierId = boost::lexical_cast<std::size_t>(line.substr(0, pos1));
                const std::string subsystemsLine = line.substr(pos1 + 1, pos2 - pos1 - 1);
                HierarchyInfo &info = id2hierarchy_[hierId];
                info.hierarchyId = hierId;
                std::vector<std::string> subsystems;
                boost::algorithm::split(subsystems, subsystemsLine, boost::algorithm::is_any_of(","));
                for (const std::string &subsystem: subsystems)
                {
                    info.subsystems.insert(subsystem);
                    BOOST_ASSERT(subsystem2id_.find(subsystem) == subsystem2id_.end());
                    subsystem2id_[subsystem] = hierId;
                }
            }
        }
        BUNSAN_EXCEPTIONS_WRAP_END()
        unistd::Fstab fstab;
        fstab.load("/proc/mounts");
        for (const unistd::MountEntry &entry: fstab)
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
                BOOST_ASSERT(!subsystems.empty());
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
                mountpoint2id_[entry.dir] = info.hierarchyId;
            }
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
        // access is atomic, no additional synchronization required
        static SystemInfoPointer instance_;
        SystemInfoPointer info = instance_;
        if (forceUpdate || !info)
        {
            info.reset(new SystemInfo);
            instance_ = info;
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