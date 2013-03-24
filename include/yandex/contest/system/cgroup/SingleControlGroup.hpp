#pragma once

#include "yandex/contest/system/cgroup/ControlGroup.hpp"
#include "yandex/contest/system/cgroup/SystemInfo.hpp"
#include "yandex/contest/system/cgroup/ProcessInfo.hpp"

#include <utility>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    struct SingleControlGroupError: virtual ControlGroupError {};
    struct SingleControlGroupNotMountedError: virtual SingleControlGroupError {};

    class SingleControlGroup: public ControlGroup
    {
    public:
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
        SystemInfoPointer systemInfo() const;
        const HierarchyInfo &hierarchyInfo() const;
        std::size_t hierarchyId() const;
        const boost::filesystem::path &controlGroup() const;

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
        virtual SingleControlGroupPointer parent()=0;

    protected:
        SingleControlGroup(const SystemInfoPointer &systemInfo,
                           const std::size_t hierarchyId,
                           const boost::filesystem::path &controlGroup);

        SingleControlGroup(const std::size_t hierarchyId,
                           const boost::filesystem::path &controlGroup);

        explicit SingleControlGroup(const ProcessHierarchyInfo &processHierarchyInfo);

        ControlGroupPointer attachChild_(const boost::filesystem::path &childControlGroup) override;
        ControlGroupPointer createChild_(const boost::filesystem::path &childControlGroup,
                                         const mode_t mode) override;
        ControlGroupPointer parent_() override;

        boost::filesystem::path fieldPath(const std::string &fieldName) const override;

        void print(std::ostream &out) const override;

        virtual void printSingle(std::ostream &out) const=0;

    private:
        const SystemInfoPointer systemInfo_;
        const HierarchyInfo &hierarchyInfo_;
        const boost::filesystem::path controlGroup_;
    };
}}}}
