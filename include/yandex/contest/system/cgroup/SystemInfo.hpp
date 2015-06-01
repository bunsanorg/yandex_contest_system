#pragma once

#include <yandex/contest/system/cgroup/Error.hpp>
#include <yandex/contest/system/cgroup/HierarchyInfo.hpp>
#include <yandex/contest/system/unistd/MountEntry.hpp>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include <functional>
#include <iostream>
#include <unordered_map>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    struct SystemInfoError: virtual Error {};
    struct SystemInfoInconsistencyError:
        virtual SystemInfoError,
        virtual InconsistencyError {};
    struct SystemInfoDuplicateHierarchiesError:
        virtual SystemInfoInconsistencyError {};
    struct SystemInfoDuplicateSubsystemsError:
        virtual SystemInfoInconsistencyError {};

    struct SystemInfoProcMountsFormatError:
        virtual SystemInfoError,
        virtual FileFormatError
    {
        typedef boost::error_info<
            struct mountEntryTag,
            unistd::MountEntry
        > mountEntry;
    };

    struct SystemInfoNoSubsystemsError:
        virtual SystemInfoProcMountsFormatError {};

    struct SystemInfoUnknownHierarchyError: virtual SystemInfoError {};

    class SystemInfo: private boost::noncopyable
    {
    private:
        typedef std::unordered_map<std::size_t, HierarchyInfo> id2hierarchy_type;
        typedef id2hierarchy_type::const_iterator map_const_iterator;
        typedef id2hierarchy_type::value_type map_value_type;

    public:
        typedef boost::shared_ptr<SystemInfo> SystemInfoPointer;
        typedef map_value_type::second_type value_type;

    private:
        struct IteratorConverter:
            std::unary_function<const map_value_type &, const value_type &>
        {
            const value_type &operator()(const map_value_type &value) const
            {
                return value.second;
            }
        };

    public:
        typedef boost::transform_iterator<
            IteratorConverter,
            map_const_iterator
        > const_iterator;

    public:
        const HierarchyInfo &byHierarchyId(const std::size_t hierarchyId) const;
        const HierarchyInfo &bySubsystem(const std::string &subsystem) const;
        const HierarchyInfo &byMountpoint(
            const boost::filesystem::path &mountpoint) const;

        const_iterator begin() const;
        const_iterator cbegin() const;
        const_iterator end() const;
        const_iterator cend() const;

    public:
        static SystemInfoPointer instance(const bool forceUpdate=false);

    private:
        SystemInfo();

        void loadHierarchies();
        void loadMountpoints();

        const HierarchyInfo &byIdNoFail(const std::size_t id) const;

    private:
        id2hierarchy_type id2hierarchy_;
        boost::unordered_map<boost::filesystem::path, std::size_t> mountpoint2id_;
        std::unordered_map<std::string, std::size_t> subsystem2id_;
    };

    typedef SystemInfo::SystemInfoPointer SystemInfoPointer;

    inline SystemInfo::const_iterator begin(const SystemInfo &systemInfo)
    {
        return systemInfo.cbegin();
    }

    inline SystemInfo::const_iterator end(const SystemInfo &systemInfo)
    {
        return systemInfo.cend();
    }

    std::ostream &operator<<(std::ostream &out, const SystemInfo &systemInfo);
}}}}
