#include "yandex/contest/system/cgroup/detail/StreamableWrapper.hpp"

#include <boost/assert.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup{namespace detail
{
#define YANDEX_CONTEST_SYSTEM_CGROUP_DETAIL_STREAMABLE_WRAPPER(PREFIX, STREAM, OPERATOR) \
    PREFIX##StreamableWrapper::ImplBase::~ImplBase() {} \
 \
    STREAM &operator OPERATOR(STREAM &stream, const PREFIX##StreamableWrapper &streamable) \
    { \
        BOOST_ASSERT(streamable.impl_); \
        streamable.impl_->dispatch(stream); \
        return stream; \
    }

    YANDEX_CONTEST_SYSTEM_CGROUP_DETAIL_STREAMABLE_WRAPPER(I, std::istream, >>)
    YANDEX_CONTEST_SYSTEM_CGROUP_DETAIL_STREAMABLE_WRAPPER(O, std::ostream, <<)
}}}}}
