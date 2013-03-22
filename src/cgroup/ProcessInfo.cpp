#include "yandex/contest/system/cgroup/ProcessInfo.hpp"
#include "yandex/contest/system/cgroup/SystemInfo.hpp"

#include "bunsan/enable_error_info.hpp"
#include "bunsan/filesystem/fstream.hpp"

#include <boost/assert.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    ProcessInfo::value_type ProcessInfo::IteratorConverter::operator()(const map_value_type &value) const
    {
        ProcessHierarchyInfo info = {
            .hierarchy = SystemInfo::instance()->byHierarchyId(value.first),
            .controlGroup = value.second,
        };
        return info;
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

    ProcessHierarchyInfo ProcessInfo::getProcessHierarchyInfo(const HierarchyInfo &info) const
    {
        const auto iter = id2controlGroup_.find(info.hierarchyId);
        if (iter == id2controlGroup_.end())
            BOOST_THROW_EXCEPTION(ProcessInfoInconsistencyError() <<
                                  ProcessInfoInconsistencyError::message(
                                      "Unable to find cgroup for hierarchy's id.") <<
                                  ProcessInfoInconsistencyError::hierarchyId(info.hierarchyId));
        ProcessHierarchyInfo processHierarchyInfo = {
            .hierarchy = info,
            .controlGroup = iter->second,
        };
        return processHierarchyInfo;
    }

    ProcessInfo ProcessInfo::fromFile(const boost::filesystem::path &path)
    {
        ProcessInfo info;
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            bunsan::filesystem::ifstream fin(path);
            std::string line;
            while (std::getline(fin, line))
            {
                const std::size_t pos1 = line.find(':');
                BOOST_ASSERT(pos1 != std::string::npos);
                const std::size_t pos2 = line.find(':', pos1 + 1);
                BOOST_ASSERT(pos2 != std::string::npos);
                BOOST_ASSERT(pos1 < pos2);
                const std::size_t hierId = boost::lexical_cast<std::size_t>(line.substr(0, pos1));
                const boost::filesystem::path controlGroup = line.substr(pos2 + 1);
                BOOST_ASSERT(info.id2controlGroup_.find(hierId) == info.id2controlGroup_.end());
                info.id2controlGroup_[hierId] = controlGroup;
                // TODO Should we check subsystems field for consistency with SystemInfo::instance()?
            }
        }
        BUNSAN_EXCEPTIONS_WRAP_END()
        return info;
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
