#include "yandex/contest/system/Trace.hpp"

#include "yandex/contest/system/unistd/DynamicLoader.hpp"

#include <boost/assert.hpp>

#include <execinfo.h>

namespace yandex{namespace contest{namespace system
{
    Trace Trace::get(const std::size_t maxSize)
    {
        Trace trace;
        trace.backtrace_.resize(maxSize);
        const std::size_t realSize = ::backtrace(&trace.backtrace_[0], maxSize);
        BOOST_ASSERT(realSize <= maxSize);
        trace.backtrace_.resize(realSize);
        return trace;
    }

    std::ostream &operator<<(std::ostream &out, const Trace &trace)
    {
        for (void *const function: trace.backtrace_)
        {
            try
            {
                out << unistd::detail::dladdr(function);
            }
            catch (unistd::DynamicLoaderError &)
            {
                out << "0x" << std::hex << function;
            }
            out << " {addr = 0x" << std::hex << function << "}\n";
        }
        return out;
    }
}}}
