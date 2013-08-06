#pragma once

#include <yandex/contest/system/cgroup/SubsystemBase.hpp>

#include <bunsan/stream_enum.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    class FreezerBase: public virtual SubsystemBase<FreezerBase>
    {
    public:
        static const std::string SUBSYSTEM_NAME;

    public:
        BUNSAN_INCLASS_STREAM_ENUM_CLASS(State,
        (
            FROZEN,
            FREEZING,
            THAWED
        ))

    public:
        State state() const;

        void freeze() const;

        void unfreeze() const;

    private:
        /*!
         * \warning State::FREEZING is not legal.
         * \note loop until state() == state_
         */
        void setState(const State state_) const;
    };

    typedef Subsystem<FreezerBase> Freezer;
}}}}
