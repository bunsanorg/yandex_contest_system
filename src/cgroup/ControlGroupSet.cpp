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

    void ControlGroupSet::swap(ControlGroupSet &controlGroupSet) noexcept
    {
        #warning TODO
    }

    void ControlGroupSet::clear()
    {
        #warning TODO
    }
}}}}
