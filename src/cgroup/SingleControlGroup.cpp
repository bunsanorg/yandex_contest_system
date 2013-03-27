#include "yandex/contest/system/cgroup/SingleControlGroup.hpp"

#include "yandex/contest/system/cgroup/detail/AttachedControlGroup.hpp"
#include "yandex/contest/system/cgroup/detail/CreatedControlGroup.hpp"

#include "yandex/contest/detail/LogHelper.hpp"

#include "bunsan/enable_error_info.hpp"

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>

#include <iterator>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    SingleControlGroup::SingleControlGroup(const SystemInfoPointer &systemInfo,
                                           const std::size_t hierarchyId,
                                           const boost::filesystem::path &controlGroup,
                                           const SingleControlGroupPointer &parent):
        systemInfo_(systemInfo),
        hierarchyInfo_(systemInfo_->byHierarchyId(hierarchyId)),
        controlGroup_(controlGroup),
        parent_(parent)
    {
        BOOST_ASSERT(parent ? parent->controlGroup() == controlGroup.parent_path() : controlGroup == "/");
        if (!hierarchyInfo_.mountpoint)
            BOOST_THROW_EXCEPTION(SingleControlGroupNotMountedError() <<
                                  SingleControlGroupNotMountedError::hierarchyId(hierarchyId));
        location_ = mountpoint() / controlGroup;
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            const boost::filesystem::file_status status =
                boost::filesystem::symlink_status(location());
            switch (status.type())
            {
            case boost::filesystem::file_not_found:
            case boost::filesystem::directory_file:
                // let child deside
                break;
            case boost::filesystem::regular_file:
                BOOST_THROW_EXCEPTION(SingleControlGroupPathToFieldError());
            default:
                BOOST_THROW_EXCEPTION(SingleControlGroupPathToUnknownError());
            }
        }
        BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(
            SingleControlGroupError::hierarchyId(hierarchyId) <<
            SingleControlGroupError::controlGroupPath(controlGroup) <<
            SingleControlGroupError::path(location()))
    }

    SingleControlGroup::~SingleControlGroup()
    {
        if (parent_)
            parent_->children_.erase(controlGroup_.filename());
    }

    const SystemInfoPointer &SingleControlGroup::systemInfo() const
    {
        return systemInfo_;
    }

    const HierarchyInfo &SingleControlGroup::hierarchyInfo() const
    {
        return hierarchyInfo_;
    }

    std::size_t SingleControlGroup::hierarchyId() const
    {
        return hierarchyInfo().id;
    }

    const boost::filesystem::path &SingleControlGroup::mountpoint() const
    {
        BOOST_ASSERT(hierarchyInfo().mountpoint);
        return hierarchyInfo().mountpoint.get();
    }

    const boost::filesystem::path &SingleControlGroup::controlGroup() const
    {
        return controlGroup_;
    }

    const boost::filesystem::path &SingleControlGroup::location() const
    {
        return location_;
    }

    ControlGroup::Tasks SingleControlGroup::tasks()
    {
        Tasks tasks_;
        readFieldByReader("tasks",
            [&tasks_](std::istream &in)
            {
                tasks_.insert(std::istream_iterator<pid_t>(in),
                              std::istream_iterator<pid_t>());
            });
        return tasks_;
    }

    void SingleControlGroup::attachTask(const pid_t pid)
    {
        STREAM_TRACE << "Attempt to attach " << pid << " to " << *this << ".";
        writeField("tasks", pid);
    }

    bool SingleControlGroup::notifyOnRelease()
    {
        return readField<int>("notify_on_release");
    }

    void SingleControlGroup::setNotifyOnRelease(const bool notifyOnRelease)
    {
        writeField<int>("notify_on_release", notifyOnRelease);
    }

    std::string SingleControlGroup::releaseAgent()
    {
        return readFieldAllRtrimmed<std::string>("release_agent");
    }

    void SingleControlGroup::setReleaseAgent(const std::string &releaseAgent)
    {
        writeField("release_agent", releaseAgent);
    }

    bool SingleControlGroup::cloneChildren()
    {
        return readField<int>("cgroup.clone_children");
    }

    void SingleControlGroup::setCloneChildren(const bool cloneChildren)
    {
        return writeField<int>("cgroup.clone_children", cloneChildren);
    }

    namespace
    {
        void checkChildControlGroup(const boost::filesystem::path &childControlGroup)
        {
            if (childControlGroup.is_absolute())
                BOOST_THROW_EXCEPTION(
                    SingleControlGroupAbsoluteControlGroupPathError() <<
                    SingleControlGroupAbsoluteControlGroupPathError::controlGroupPath(childControlGroup));
        }
    }

    SingleControlGroupPointer SingleControlGroup::attachChild(const boost::filesystem::path &childControlGroup)
    {
        checkChildControlGroup(childControlGroup);
        SingleControlGroupPointer current(this);
        for (const boost::filesystem::path &i: childControlGroup)
            current = current->attachDirectChild(i);
        return current;
    }

    SingleControlGroupPointer SingleControlGroup::createChild(const boost::filesystem::path &childControlGroup)
    {
        return createChild(childControlGroup, 0777);
    }

    SingleControlGroupPointer SingleControlGroup::createChild(const boost::filesystem::path &childControlGroup,
                                                              const mode_t mode)
    {
        checkChildControlGroup(childControlGroup);
        SingleControlGroupPointer current(this);
        for (const boost::filesystem::path &i: childControlGroup)
            current = current->createDirectChild(i, mode);
        return current;
    }

    SingleControlGroupPointer SingleControlGroup::parent()
    {
        return parent_;
    }

    ControlGroupPointer SingleControlGroup::attachChild__(const boost::filesystem::path &childControlGroup)
    {
        return boost::static_pointer_cast<ControlGroup>(attachChild(childControlGroup));
    }

    ControlGroupPointer SingleControlGroup::createChild__(const boost::filesystem::path &childControlGroup,
                                                         const mode_t mode)
    {
        return boost::static_pointer_cast<ControlGroup>(createChild(childControlGroup, mode));
    }

    ControlGroupPointer SingleControlGroup::parent__()
    {
        return boost::static_pointer_cast<ControlGroup>(parent());
    }

    SingleControlGroupPointer SingleControlGroup::forProcessHierarchyInfo(
        const ProcessHierarchyInfo &processHierarchyInfo)
    {
        return attach(processHierarchyInfo.hierarchy.id, processHierarchyInfo.controlGroup);
    }

    SingleControlGroupPointer SingleControlGroup::forProcessInfo(const std::size_t hierarchyId,
                                                                 const ProcessInfo &processInfo)
    {
        return forProcessHierarchyInfo(processInfo.byHierarchyId(hierarchyId));
    }

    SingleControlGroupPointer SingleControlGroup::forPid(const std::size_t hierarchyId, const pid_t pid)
    {
        return forProcessInfo(hierarchyId, ProcessInfo::forPid(pid));
    }

    SingleControlGroupPointer SingleControlGroup::forSelf(const std::size_t hierarchyId)
    {
        return forProcessInfo(hierarchyId, ProcessInfo::forSelf());
    }

    SingleControlGroupPointer SingleControlGroup::root(const std::size_t hierarchyId)
    {
        return SingleControlGroupPointer(
            new detail::AttachedControlGroup(SystemInfo::instance(), hierarchyId, "/", SingleControlGroupPointer()));
    }

    SingleControlGroupPointer SingleControlGroup::attach(const std::size_t hierarchyId,
                                                         const boost::filesystem::path &controlGroup)
    {
        if (controlGroup.is_relative())
            BOOST_THROW_EXCEPTION(
                SingleControlGroupRelativeControlGroupError() <<
                SingleControlGroupRelativeControlGroupError::controlGroupPath(controlGroup));
        return root(hierarchyId)->attachChild(controlGroup.relative_path());
    }

    boost::filesystem::path SingleControlGroup::fieldPath__(const std::string &fieldName) const
    {
        return hierarchyInfo().mountpoint.get() / controlGroup() / fieldName;
    }

    void SingleControlGroup::print(std::ostream &out) const
    {
        out << "{ ";
        out << "hierarchy = " << hierarchyInfo() << ", ";
        out << "cgroup = " << controlGroup() << ", ";
        printSingle(out);
        out << " }";
    }

    SingleControlGroupPointer SingleControlGroup::attachDirectChild(
        const boost::filesystem::path &childControlGroup)
    {
        BOOST_ASSERT(childControlGroup.is_relative());
        BOOST_ASSERT(childControlGroup == childControlGroup.filename());
        if (childControlGroup.empty() || childControlGroup == ".")
        {
            return SingleControlGroupPointer(this);
        }
        else if (childControlGroup == "..")
        {
            SingleControlGroupPointer ret = parent();
            if (!ret)
                ret.reset(this);
            return ret;
        }
        else
        {
            auto iter = children_.find(childControlGroup);
            if (iter == children_.end())
            {
                const SingleControlGroupPointer cgroup(
                    new detail::AttachedControlGroup(systemInfo(),
                                                     hierarchyId(),
                                                     controlGroup() / childControlGroup,
                                                     SingleControlGroupPointer(this)));
                const auto iter_inserted = children_.emplace(childControlGroup, cgroup.get());
                BOOST_ASSERT(iter_inserted.second);
                return cgroup;
            }
            else
            {
                return SingleControlGroupPointer(iter->second);
            }
        }
    }

    SingleControlGroupPointer SingleControlGroup::createDirectChild(
        const boost::filesystem::path &childControlGroup, const mode_t mode)
    {
        BOOST_ASSERT(childControlGroup.is_relative());
        BOOST_ASSERT(childControlGroup == childControlGroup.filename());
        if (childControlGroup == "." || childControlGroup == "..")
            BOOST_THROW_EXCEPTION(SingleControlGroupPathError() <<
                                  SingleControlGroupPathError::controlGroupPath(controlGroup() / childControlGroup) <<
                                  SingleControlGroupPathError::message("Invalid path."));
        if (childControlGroup.empty())
            BOOST_THROW_EXCEPTION(SingleControlGroupEmptyControlGroupPathError() <<
                                  SingleControlGroupEmptyControlGroupPathError::controlGroupPath(controlGroup()));
        if (children_.find(childControlGroup) != children_.end())
            BOOST_THROW_EXCEPTION(SingleControlGroupExistsError() <<
                                  SingleControlGroupExistsError::controlGroupPath(controlGroup() / childControlGroup));
        const SingleControlGroupPointer cgroup(
            new detail::CreatedControlGroup(systemInfo(),
                                            hierarchyId(),
                                            controlGroup() / childControlGroup, mode,
                                            SingleControlGroupPointer(this)));
        const auto iter_inserted = children_.emplace(childControlGroup, cgroup.get());
        BOOST_ASSERT(iter_inserted.second);
        return cgroup;
    }
}}}}
