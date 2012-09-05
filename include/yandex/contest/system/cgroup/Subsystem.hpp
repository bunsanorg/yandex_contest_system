#pragma once

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    class ControlGroup;

    template <typename Base>
    class Subsystem: public virtual Base
    {
    public:
        explicit Subsystem(ControlGroup &controlGroup):
            controlGroup_(controlGroup) {}

        Subsystem(const Subsystem &)=default;
        Subsystem &operator=(const Subsystem &)=default;

        ControlGroup &controlGroup() const override
        {
            return controlGroup_;
        }

    private:
        ControlGroup &controlGroup_;
    };
}}}}
