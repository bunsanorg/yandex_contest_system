#pragma once

#include "yandex/contest/detail/ForwardHeader.hpp"

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    YANDEX_CONTEST_INTRUSIVE_PTR_FWD_DECLARE(ControlGroup);

    class SingleControlGroup;
    typedef boost::intrusive_ptr<SingleControlGroup> SingleControlGroupPointer;

    class MultipleControlGroup;
    typedef boost::intrusive_ptr<SingleControlGroup> MultipleControlGroupPointer;
}}}}

#include "yandex/contest/detail/ForwardFooter.hpp"
