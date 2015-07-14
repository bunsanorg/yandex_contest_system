#pragma once

#include <yandex/contest/detail/ForwardHeader.hpp>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {

YANDEX_CONTEST_INTRUSIVE_PTR_FWD_DECLARE(ControlGroup);

class SingleControlGroup;
using SingleControlGroupPointer = boost::intrusive_ptr<SingleControlGroup>;

class MultipleControlGroup;
using MultipleControlGroupPointer = boost::intrusive_ptr<MultipleControlGroup>;

}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex

#include <yandex/contest/detail/ForwardFooter.hpp>
