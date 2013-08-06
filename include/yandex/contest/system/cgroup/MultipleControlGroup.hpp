#pragma once

#include <yandex/contest/system/cgroup/ControlGroup.hpp>
#include <yandex/contest/system/cgroup/SingleControlGroup.hpp>

#include <unordered_map>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    struct MultipleControlGroupError: virtual ControlGroupError {};
    struct EmptyMultipleControlGroupError: virtual MultipleControlGroupError {};

    struct MultipleControlGroupHierarchyError: virtual MultipleControlGroupError {};
    struct MultipleControlGroupHierarchyConflictError: virtual MultipleControlGroupHierarchyError {};
    struct MultipleControlGroupHierarchyNotFoundError: virtual MultipleControlGroupHierarchyError {};

    struct MultipleControlGroupFieldError: virtual MultipleControlGroupError, virtual ControlGroupFieldError {};
    struct MultipleControlGroupFieldValueError: virtual MultipleControlGroupFieldError {};
    struct MultipleControlGroupFieldValueConflictError: virtual MultipleControlGroupFieldValueError {};

    class MultipleControlGroup: public ControlGroup
    {
    public:
        /// For all available hierarchies.
        static MultipleControlGroupPointer forPid(const pid_t pid);

        /// For all available hierarchies.
        static MultipleControlGroupPointer forSelf();

        /// For all available hierarchies.
        static MultipleControlGroupPointer root();

        /// For all available hierarchies.
        static MultipleControlGroupPointer attach(const boost::filesystem::path &controlGroup);

        /// Unite specified cgroups.
        template <typename Iter>
        static MultipleControlGroupPointer unite(Iter begin, const Iter end)
        {
            const MultipleControlGroupPointer cgroup(new MultipleControlGroup);
            for (; begin != end; ++begin)
                cgroup->add(*begin);
            return cgroup;
        }

        /// For specified hierarchies.
        template <typename Iter>
        static MultipleControlGroupPointer forPid(const pid_t pid, Iter begin, const Iter end)
        {
            const MultipleControlGroupPointer cgroup(new MultipleControlGroup);
            for (; begin != end; ++begin)
                cgroup->add(SingleControlGroup::forPid(*begin, pid));
            return cgroup;
        }

        /// For specified hierarchies.
        template <typename Iter>
        static MultipleControlGroupPointer forSelf(Iter begin, const Iter end)
        {
            const MultipleControlGroupPointer cgroup(new MultipleControlGroup);
            for (; begin != end; ++begin)
                cgroup->add(SingleControlGroup::forSelf(*begin));
            return cgroup;
        }

        /// For specified hierarchies.
        template <typename Iter>
        static MultipleControlGroupPointer root(Iter begin, const Iter end)
        {
            const MultipleControlGroupPointer cgroup(new MultipleControlGroup);
            for (; begin != end; ++begin)
                cgroup->add(SingleControlGroup::root(*begin));
            return cgroup;
        }

        /// For specified hierarchies.
        template <typename Iter>
        static MultipleControlGroupPointer attach(const boost::filesystem::path &controlGroup,
                                                  Iter begin, const Iter end)
        {
            const MultipleControlGroupPointer cgroup(new MultipleControlGroup);
            for (; begin != end; ++begin)
                cgroup->add(SingleControlGroup::attach(*begin));
            return cgroup;
        }

    public:
        /// \throws MultipleControlGroupHierarchyConflictError
        void add(const SingleControlGroupPointer &cgroup);

        /// \throws MultipleControlGroupHierarchyNotFoundError
        SingleControlGroupPointer replace(const SingleControlGroupPointer &cgroup);

        /// \throws MultipleControlGroupHierarchyNotFoundError
        SingleControlGroupPointer remove(const std::size_t hierarchyId);

        /// \return nullptr if not present
        SingleControlGroupPointer find(const std::size_t hierarchyId) const;

        Tasks tasks() override;

        void attachTask(const pid_t pid) override;

        bool notifyOnRelease() override;
        void setNotifyOnRelease(const bool notifyOnRelease=true) override;

        std::string releaseAgent() override;

        void setReleaseAgent(const std::string &releaseAgent) override;

        bool cloneChildren() override;
        void setCloneChildren(const bool cloneChildren=true) override;

        MultipleControlGroupPointer attachChild(const boost::filesystem::path &childControlGroup);
        MultipleControlGroupPointer createChild(const boost::filesystem::path &childControlGroup);
        MultipleControlGroupPointer createChild(const boost::filesystem::path &childControlGroup,
                                                const mode_t mode);
        MultipleControlGroupPointer parent();

    protected:
        MultipleControlGroup()=default;

        ControlGroupPointer attachChild__(const boost::filesystem::path &childControlGroup) override;
        ControlGroupPointer createChild__(const boost::filesystem::path &childControlGroup,
                                          const mode_t mode) override;
        ControlGroupPointer parent__() override;

        boost::filesystem::path fieldPath__(const std::string &fieldName) const override;

        void print(std::ostream &out) const override;

    private:
        std::unordered_map<std::size_t, SingleControlGroupPointer> id2cgroup_;

        /// field path cache
        mutable std::unordered_map<std::string, boost::filesystem::path> fieldName2path_;
    };
}}}}
