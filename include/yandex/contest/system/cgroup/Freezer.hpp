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

    class FreezerBase: public SubsystemBase<FreezerBase>
    {
    public:
        static const std::string SUBSYSTEM_NAME;

    public:
        typedef freezer_detail::State State;

    public:
        State state() const;

        /*!
         * \warning State::FREEZING is not legal.
         * \note loop until FROZEN THAWED
         */
        void setState(const State state) const;
    };

    typedef SubsystemBase<FreezerBase> Freezer;
}}}}
