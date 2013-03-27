#include "yandex/contest/system/cgroup/MultipleControlGroup.hpp"

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    MultipleControlGroupPointer MultipleControlGroup::forPid(const pid_t pid)
    {
        const MultipleControlGroupPointer cgroup(new MultipleControlGroup);
        for (const HierarchyInfo &hierarchyInfo: *SystemInfo::instance())
            cgroup->add(SingleControlGroup::forPid(hierarchyInfo.id, pid));
        return cgroup;
    }

    MultipleControlGroupPointer MultipleControlGroup::forSelf()
    {
        const MultipleControlGroupPointer cgroup(new MultipleControlGroup);
        for (const HierarchyInfo &hierarchyInfo: *SystemInfo::instance())
            cgroup->add(SingleControlGroup::forSelf(hierarchyInfo.id));
        return cgroup;
    }

    MultipleControlGroupPointer MultipleControlGroup::root()
    {
        const MultipleControlGroupPointer cgroup(new MultipleControlGroup);
        for (const HierarchyInfo &hierarchyInfo: *SystemInfo::instance())
            cgroup->add(SingleControlGroup::root(hierarchyInfo.id));
        return cgroup;
    }

    MultipleControlGroupPointer MultipleControlGroup::attach(const boost::filesystem::path &controlGroup)
    {
        const MultipleControlGroupPointer cgroup(new MultipleControlGroup);
        for (const HierarchyInfo &hierarchyInfo: *SystemInfo::instance())
            cgroup->add(SingleControlGroup::attach(hierarchyInfo.id, controlGroup));
        return cgroup;
    }

    void MultipleControlGroup::add(const SingleControlGroupPointer &cgroup)
    {
        BOOST_ASSERT(cgroup);
        fieldName2path_.clear();
        const std::size_t hierarchyId = cgroup->hierarchyId();
        if (id2cgroup_.find(hierarchyId) != id2cgroup_.end())
            BOOST_THROW_EXCEPTION(MultipleControlGroupHierarchyConflictError() <<
                                  MultipleControlGroupHierarchyConflictError::hierarchyId(hierarchyId));
        id2cgroup_[hierarchyId] = cgroup;
    }

    SingleControlGroupPointer MultipleControlGroup::replace(const SingleControlGroupPointer &cgroup)
    {
        BOOST_ASSERT(cgroup);
        fieldName2path_.clear();
        const std::size_t hierarchyId = cgroup->hierarchyId();
        auto iter = id2cgroup_.find(hierarchyId);
        if (iter == id2cgroup_.end())
            BOOST_THROW_EXCEPTION(MultipleControlGroupHierarchyNotFoundError() <<
                                  MultipleControlGroupHierarchyNotFoundError::hierarchyId(hierarchyId));
        const SingleControlGroupPointer ret = iter->second;
        iter->second = cgroup;
        return ret;
    }

    SingleControlGroupPointer MultipleControlGroup::remove(const std::size_t hierarchyId)
    {
        fieldName2path_.clear();
        const auto iter = id2cgroup_.find(hierarchyId);
        if (iter == id2cgroup_.end())
            BOOST_THROW_EXCEPTION(MultipleControlGroupHierarchyNotFoundError() <<
                                  MultipleControlGroupHierarchyNotFoundError::hierarchyId(hierarchyId));
        const SingleControlGroupPointer ret = iter->second;
        id2cgroup_.erase(iter);
        return ret;
    }

    SingleControlGroupPointer MultipleControlGroup::find(const std::size_t hierarchyId) const
    {
        SingleControlGroupPointer ret;
        const auto iter = id2cgroup_.find(hierarchyId);
        if (iter != id2cgroup_.end())
            ret = iter->second;
        return ret;
    }

    ControlGroup::Tasks MultipleControlGroup::tasks()
    {
        // TODO
    }

    void MultipleControlGroup::attachTask(const pid_t pid)
    {
        for (const std::pair<std::size_t, SingleControlGroupPointer> &id_cgroup: id2cgroup_)
            id_cgroup.second->attachTask(pid);
    }

    namespace
    {
        template <typename Ret, typename Iter>
        Ret foldFields(Iter begin, const Iter end,
                       Ret (SingleControlGroup::*getter)(), const std::string &fieldName)
        {
            if (begin == end)
                BOOST_THROW_EXCEPTION(EmptyMultipleControlGroupError());
            bool initialized = false;
            Ret value;
            for (; begin != end; ++begin)
            {
                const Ret nextValue = (begin->second.get()->*getter)();
                if (initialized)
                {
                    if (value != nextValue)
                        BOOST_THROW_EXCEPTION(
                            MultipleControlGroupFieldValueConflictError() <<
                            MultipleControlGroupFieldValueConflictError::fieldName(fieldName));
                }
                else
                {
                    value = nextValue;
                    initialized = true;
                }
            }
            BOOST_ASSERT(initialized);
            return value;
        }
    }

    bool MultipleControlGroup::notifyOnRelease()
    {
        return foldFields(id2cgroup_.begin(), id2cgroup_.end(),
                          &SingleControlGroup::notifyOnRelease, "notify_on_release");
    }

    void MultipleControlGroup::setNotifyOnRelease(const bool notifyOnRelease)
    {
        for (const std::pair<std::size_t, SingleControlGroupPointer> &id_cgroup: id2cgroup_)
            id_cgroup.second->setNotifyOnRelease(notifyOnRelease);
    }

    std::string MultipleControlGroup::releaseAgent()
    {
        return foldFields(id2cgroup_.begin(), id2cgroup_.end(),
                          &SingleControlGroup::releaseAgent, "release_agent");
    }

    void MultipleControlGroup::setReleaseAgent(const std::string &releaseAgent)
    {
        for (const std::pair<std::size_t, SingleControlGroupPointer> &id_cgroup: id2cgroup_)
            id_cgroup.second->setReleaseAgent(releaseAgent);
    }

    bool MultipleControlGroup::cloneChildren()
    {
        return foldFields(id2cgroup_.begin(), id2cgroup_.end(),
                          &SingleControlGroup::cloneChildren, "cgroup.clone_children");
    }

    void MultipleControlGroup::setCloneChildren(const bool cloneChildren)
    {
        for (const std::pair<std::size_t, SingleControlGroupPointer> &id_cgroup: id2cgroup_)
            id_cgroup.second->setCloneChildren(cloneChildren);
    }

    MultipleControlGroupPointer MultipleControlGroup::attachChild(const boost::filesystem::path &childControlGroup)
    {
        const MultipleControlGroupPointer child(new MultipleControlGroup);
        for (const std::pair<std::size_t, SingleControlGroupPointer> &id_cgroup: id2cgroup_)
            child->id2cgroup_[id_cgroup.first] = id_cgroup.second->attachChild(childControlGroup);
        return child;
    }

    MultipleControlGroupPointer MultipleControlGroup::createChild(const boost::filesystem::path &childControlGroup)
    {
        const MultipleControlGroupPointer child(new MultipleControlGroup);
        for (const std::pair<std::size_t, SingleControlGroupPointer> &id_cgroup: id2cgroup_)
            child->id2cgroup_[id_cgroup.first] = id_cgroup.second->createChild(childControlGroup);
        return child;
    }

    MultipleControlGroupPointer MultipleControlGroup::createChild(const boost::filesystem::path &childControlGroup,
                                                                  const mode_t mode)
    {
        const MultipleControlGroupPointer child(new MultipleControlGroup);
        for (const std::pair<std::size_t, SingleControlGroupPointer> &id_cgroup: id2cgroup_)
            child->id2cgroup_[id_cgroup.first] = id_cgroup.second->createChild(childControlGroup, mode);
        return child;
    }

    MultipleControlGroupPointer MultipleControlGroup::parent()
    {
        const MultipleControlGroupPointer child(new MultipleControlGroup);
        for (const std::pair<std::size_t, SingleControlGroupPointer> &id_cgroup: id2cgroup_)
        {
            const SingleControlGroupPointer parent = id_cgroup.second->parent();
            if (!parent)
                return MultipleControlGroupPointer();
            child->id2cgroup_[id_cgroup.first] = parent;
        }
        return child;
    }

    boost::filesystem::path MultipleControlGroup::fieldPath__(const std::string &fieldName) const
    {
        const auto iter = fieldName2path_.find(fieldName);
        if (iter == fieldName2path_.end())
        {
            for (const std::pair<std::size_t, SingleControlGroupPointer> &id_cgroup: id2cgroup_)
            {
                try
                {
                    const boost::filesystem::path path = id_cgroup.second->fieldPath(fieldName);
                    fieldName2path_[fieldName] = path;
                    return path;
                }
                catch (ControlGroupFieldDoesNotExistError &)
                {
                    // continue
                }
            }
            BOOST_THROW_EXCEPTION(ControlGroupFieldDoesNotExistError() <<
                                  ControlGroupFieldDoesNotExistError::fieldName(fieldName));
        }
        else
        {
            return iter->second;
        }
    }

    ControlGroupPointer MultipleControlGroup::attachChild__(const boost::filesystem::path &childControlGroup)
    {
        return boost::static_pointer_cast<ControlGroup>(attachChild(childControlGroup));
    }

    ControlGroupPointer MultipleControlGroup::createChild__(const boost::filesystem::path &childControlGroup,
                                                            const mode_t mode)
    {
        return boost::static_pointer_cast<ControlGroup>(createChild(childControlGroup, mode));
    }

    ControlGroupPointer MultipleControlGroup::parent__()
    {
        return boost::static_pointer_cast<ControlGroup>(parent());
    }

    void MultipleControlGroup::print(std::ostream &out) const
    {
        out << "{ ";
        bool first = true;
        for (const std::pair<std::size_t, SingleControlGroupPointer> &id_cgroup: id2cgroup_)
        {
            if (!first)
                out << ", ";
            first = false;
            out << *id_cgroup.second;
        }
        out << " }";
    }
}}}}
