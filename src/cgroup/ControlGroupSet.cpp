#include "yandex/contest/system/cgroup/ControlGroupSet.hpp"

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    ControlGroupSet::ControlGroupSet(ControlGroupSet &&controlGroupSet)
    {
        swap(controlGroupSet);
        controlGroupSet.clear();
    }

    ControlGroupSet &ControlGroupSet::operator=(ControlGroupSet &&controlGroupSet)
    {
        swap(controlGroupSet);
        controlGroupSet.clear();
        return *this;
    }

    void ControlGroupSet::insert(ControlGroup &&controlGroup)
    {
        #warning TODO
    }

    void ControlGroupSet::clear()
    {
        #warning TODO
    }

    void ControlGroupSet::attachTask(const pid_t pid)
    {
        #warning TODO
    }

    ControlGroupSet ControlGroupSet::createChild()
    {
        #warning TODO
    }

    ControlGroupSet ControlGroupSet::attachChild()
    {
        #warning TODO
    }

    void ControlGroupSet::swap(ControlGroupSet &controlGroupSet) noexcept
    {
        #warning TODO
    }
}}}}
