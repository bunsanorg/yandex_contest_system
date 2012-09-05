#pragma once

#include "yandex/contest/system/cgroup/SubsystemBase.hpp"

#include "yandex/contest/StreamEnum.hpp"

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    namespace freezer_detail
    {
        YANDEX_CONTEST_STREAM_ENUM_CLASS(State,
        (
            FROZEN,
            FREEZING,
            THAWED
        ))
    }

    class FreezerBase: public virtual SubsystemBase<FreezerBase>
    {
    public:
        static const std::string SUBSYSTEM_NAME;

    public:
        typedef freezer_detail::State State;

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
