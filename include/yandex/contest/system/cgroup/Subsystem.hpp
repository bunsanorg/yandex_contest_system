#pragma once

#include "yandex/contest/system/cgroup/Forward.hpp"

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    template <typename Base>
    class Subsystem: public virtual Base
    {
    public:
        explicit Subsystem(const ControlGroupPointer &controlGroup):
            controlGroup_(controlGroup) {}

        Subsystem(const Subsystem &)=default;
        Subsystem &operator=(const Subsystem &)=default;
        Subsystem(Subsystem &&)=default;
        Subsystem &operator=(Subsystem &&)=default;

        ControlGroup &controlGroup() const override
        {
            return *controlGroup_;
        }

    private:
        ControlGroupPointer controlGroup_;
    };
}}}}
