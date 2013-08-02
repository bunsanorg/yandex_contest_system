#pragma once

#include "yandex/contest/system/cgroup/ControlGroup.hpp"
#include "yandex/contest/system/cgroup/SystemInfo.hpp"
#include "yandex/contest/system/cgroup/ProcessInfo.hpp"

#include <boost/unordered_map.hpp>

#include <utility>

#include <sys/types.h>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    struct SingleControlGroupError: virtual ControlGroupError {};

    struct SingleControlGroupAttachError: virtual SingleControlGroupError {};
    struct SingleControlGroupNotExistsError: virtual SingleControlGroupAttachError {};

    struct SingleControlGroupCreateError: virtual SingleControlGroupError {};
    struct SingleControlGroupExistsError: virtual SingleControlGroupCreateError {};

    struct SingleControlGroupNotMountedError: virtual SingleControlGroupError {};

    struct SingleControlGroupPathError: virtual SingleControlGroupError {};
    struct SingleControlGroupEmptyControlGroupPathError: virtual SingleControlGroupPathError {};
    struct SingleControlGroupAbsoluteControlGroupPathError: virtual SingleControlGroupPathError {};
    struct SingleControlGroupRelativeControlGroupError: virtual SingleControlGroupPathError {};
    struct SingleControlGroupPathToFieldError: virtual SingleControlGroupPathError{};
    struct SingleControlGroupPathToUnknownError: virtual SingleControlGroupPathError{};

    class SingleControlGroup: public ControlGroup
    {
    public:
        ~SingleControlGroup() override;

        static SingleControlGroupPointer forProcessHierarchyInfo(
            const ProcessHierarchyInfo &processHierarchyInfo);

        static SingleControlGroupPointer forProcessInfo(const std::size_t hierarchyId,
                                                        const ProcessInfo &processInfo);

        static SingleControlGroupPointer forPid(const std::size_t hierarchyId, const pid_t pid);

        static SingleControlGroupPointer forSelf(const std::size_t hierarchyId);

        static SingleControlGroupPointer root(const std::size_t hierarchyId);

        static SingleControlGroupPointer attach(const std::size_t hierarchyId,
                                                const boost::filesystem::path &controlGroup);

    public:
        const SystemInfoPointer &systemInfo() const;
        const HierarchyInfo &hierarchyInfo() const;
        std::size_t hierarchyId() const;
        const boost::filesystem::path &mountpoint() const;
        const boost::filesystem::path &controlGroup() const;
        const boost::filesystem::path &location() const;

        Tasks tasks() override;

        void attachTask(const pid_t pid) override;

        bool notifyOnRelease() override;
        void setNotifyOnRelease(const bool notifyOnRelease=true) override;

        std::string releaseAgent() override;

        void setReleaseAgent(const std::string &releaseAgent) override;

        bool cloneChildren() override;
        void setCloneChildren(const bool cloneChildren=true) override;

        SingleControlGroupPointer attachChild(const boost::filesystem::path &childControlGroup);
        SingleControlGroupPointer createChild(const boost::filesystem::path &childControlGroup);
        SingleControlGroupPointer createChild(const boost::filesystem::path &childControlGroup,
                                              const mode_t mode);
        SingleControlGroupPointer parent();

    protected:
        /// \throws SingleControlGroupPathError if location() is neither directory nor not_found.
        SingleControlGroup(const SystemInfoPointer &systemInfo,
                           const std::size_t hierarchyId,
                           const boost::filesystem::path &controlGroup,
                           const SingleControlGroupPointer &parent);

        ControlGroupPointer attachChild__(const boost::filesystem::path &childControlGroup) override;
        ControlGroupPointer createChild__(const boost::filesystem::path &childControlGroup,
                                          const mode_t mode) override;
        ControlGroupPointer parent__() override;

        boost::filesystem::path fieldPath(const std::string &fieldName) const override;

        void print(std::ostream &out) const override;

        virtual void printSingle(std::ostream &out) const=0;

    private:
        SingleControlGroupPointer attachDirectChild(const boost::filesystem::path &childControlGroup);
        SingleControlGroupPointer createDirectChild(const boost::filesystem::path &childControlGroup,
                                                    const mode_t mode);

    private:
        const SystemInfoPointer systemInfo_;
        const HierarchyInfo &hierarchyInfo_;
        const boost::filesystem::path controlGroup_;
        const SingleControlGroupPointer parent_;
        boost::unordered_map<boost::filesystem::path, SingleControlGroup *> children_;
        boost::filesystem::path location_;
    };
}}}}
