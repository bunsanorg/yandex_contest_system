#pragma once

#include "yandex/contest/StreamEnum.hpp"

namespace yandex{namespace contest{namespace system{namespace lxc{namespace lxc_detail
{
    /*!
     * \brief LXC states.
     *
     * \warning Get in sync with ::lxc_state_t.
     */
    YANDEX_CONTEST_STREAM_ENUM_CLASS(State,
    (
        STOPPED,
        STARTING,
        RUNNING,
        STOPPING,
        ABORTING,
        FREEZING,
        FROZEN,
        THAWED,
        MAX_STATE
    ))
}}}}}
