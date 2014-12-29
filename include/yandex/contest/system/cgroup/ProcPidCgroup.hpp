#pragma once

#include <yandex/contest/system/cgroup/Error.hpp>

#include <boost/filesystem/path.hpp>

#include <string>
#include <unordered_set>
#include <vector>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    struct ProcPidCgroupFormatError:
        virtual FileFormatError {};
    struct ProcPidCgroupLineFormatError:
        virtual ProcPidCgroupFormatError,
        virtual FileLineFormatError {};
    struct ProcPidCgroupNotEnoughFieldsError:
        virtual ProcPidCgroupLineFormatError {};

    struct ProcPidCgroup
    {
        struct Entry
        {
            std::size_t hierarchyId = 0;
            std::unordered_set<std::string> subsystems;
            boost::filesystem::path controlGroup;
        };

        std::vector<Entry> entries;

        void load(const boost::filesystem::path &path);
    };

    inline std::vector<ProcPidCgroup::Entry>::iterator begin(
        ProcPidCgroup &cgroup)
    {
        return cgroup.entries.begin();
    }

    inline std::vector<ProcPidCgroup::Entry>::const_iterator begin(
        const ProcPidCgroup &cgroup)
    {
        return cgroup.entries.begin();
    }

    inline std::vector<ProcPidCgroup::Entry>::iterator end(
        ProcPidCgroup &cgroup)
    {
        return cgroup.entries.end();
    }

    inline std::vector<ProcPidCgroup::Entry>::const_iterator end(
        const ProcPidCgroup &cgroup)
    {
        return cgroup.entries.end();
    }
}}}}
