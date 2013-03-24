#pragma once

#include "yandex/contest/system/cgroup/Error.hpp"
#include "yandex/contest/system/cgroup/ProcessHierarchyInfo.hpp"

#include <iostream>
#include <functional>
#include <unordered_map>

#include <boost/iterator/transform_iterator.hpp>

#include <sys/types.h>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    struct ProcessInfoError: virtual Error {};
    struct ProcessInfoInconsistencyError: virtual ProcessInfoError, virtual InconsistencyError {};
    struct ProcessInfoDuplicateHierarchiesError: virtual ProcessInfoError, virtual FileFormatError {};

    class ProcessInfo
    {
    private:
        typedef std::unordered_map<std::size_t, boost::filesystem::path> id2controlGroupType;
        typedef id2controlGroupType::const_iterator map_const_iterator;
        typedef id2controlGroupType::value_type map_value_type;

    public:
        typedef ProcessHierarchyInfo value_type;

    private:
        struct IteratorConverter: std::unary_function<const map_value_type &, value_type>
        {
            value_type operator()(const map_value_type &value) const;
        };

    public:
        typedef boost::transform_iterator<IteratorConverter, map_const_iterator> const_iterator;

    public:
        ProcessInfo()=default;
        ProcessInfo(const ProcessInfo &)=default;
        ProcessInfo(ProcessInfo &&)=default;
        ProcessInfo &operator=(const ProcessInfo &)=default;
        ProcessInfo &operator=(ProcessInfo &&)=default;

        void swap(ProcessInfo &processInfo) noexcept;

        ProcessHierarchyInfo byHierarchyId(const std::size_t hierarchyId) const;
        ProcessHierarchyInfo bySubsystem(const std::string &subsystem) const;
        ProcessHierarchyInfo byMountpoint(const boost::filesystem::path &mountpoint) const;

        const_iterator begin() const;
        const_iterator cbegin() const;
        const_iterator end() const;
        const_iterator cend() const;

    public:
        static ProcessInfo fromFile(const boost::filesystem::path &path);
        static ProcessInfo forPid(const pid_t pid);
        static ProcessInfo forSelf();

    private:
        ProcessHierarchyInfo getProcessHierarchyInfo(const HierarchyInfo &info) const;

    private:
        id2controlGroupType id2controlGroup_;
    };

    inline void swap(ProcessInfo &a, ProcessInfo &b) noexcept
    {
        a.swap(b);
    }

    inline ProcessInfo::const_iterator begin(const ProcessInfo &processInfo)
    {
        return processInfo.cbegin();
    }

    inline ProcessInfo::const_iterator end(const ProcessInfo &processInfo)
    {
        return processInfo.cend();
    }

    std::ostream &operator<<(std::ostream &out, const ProcessInfo &processInfo);
}}}}
