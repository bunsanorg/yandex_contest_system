#pragma once

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    class ControlGroup;

    template <typename Base>
    class Subsystem: public Base
    {
    public:
        explicit Subsystem(ControlGroup &controlGroup):
            controlGroup_(controlGroup) {}

        Subsystem(const Subsystem &)=default;
        Subsystem &operator=(const Subsystem &)=default;

        ControlGroup &controlGroup() const
        {
            return controlGroup_;
        }

    private:
        ControlGroup &controlGroup_;
    };
}}}}
#if 0
#pragma once

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    class ControlGroup;

    class BasicSubsystem
    {
    public:
        explicit BasicSubsystem(ControlGroup &controlGroup):
            controlGroup_(controlGroup) {}

        BasicSubsystem(const BasicSubsystem &)=default;
        BasicSubsystem &operator=(const BasicSubsystem &)=default;

        ControlGroup &controlGroup() const
        {
            return controlGroup_;
        }

    private:
        ControlGroup &controlGroup_;
    };

    template <typename Base>
    class Subsystem: public Base, BasicSubsystem
    {
    public:
        explicit Subsystem(ControlGroup &controlGroup):
            BasicSubsystem(controlGroup) {}
    };
}}}}
#endif