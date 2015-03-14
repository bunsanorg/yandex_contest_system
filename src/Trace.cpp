#include <yandex/contest/system/Trace.hpp>

#include <yandex/contest/system/unistd/DynamicLoader.hpp>

#include <yandex/contest/StreamLog.hpp>
#include <yandex/contest/SystemError.hpp>

#include <boost/assert.hpp>

#include <cstdlib>
#include <cstring>

#include <execinfo.h>

namespace yandex{namespace contest{namespace system
{
    Trace Trace::get(const std::size_t maxSize)
    {
        Trace trace;
        trace.resize(maxSize);
        const std::size_t realSize = ::backtrace(&trace[0], maxSize);
        BOOST_ASSERT(realSize <= maxSize);
        trace.resize(realSize);
        trace.erase(trace.begin());
        return trace;
    }

    void Trace::handler(int sig, siginfo_t * /*siginfo*/, void * /*context*/)
    {
        STREAM_ERROR << "Processing signal: " << strsignal(sig) << '\n' <<
                        "backtrace:\n" << get();
        exit(EXIT_FAILURE);
    }

    void Trace::handle(const int sig, void (*h)(int, siginfo_t *, void *))
    {
        struct sigaction sigact;
        sigact.sa_sigaction = h;
        sigact.sa_flags = SA_RESTART | SA_SIGINFO;
        if (sigaction(sig, &sigact, nullptr) < 0)
            BOOST_THROW_EXCEPTION(SystemError("sigaction"));
    }

    std::ostream &operator<<(std::ostream &out, const Trace &trace)
    {
        for (void *const function: trace)
        {
            try
            {
                out << unistd::detail::dladdr(function) <<
                       " {addr = 0x" << std::hex << function << "}\n";
            }
            catch (unistd::DynamicLoaderError &)
            {
                out << "0x" << std::hex << function << "\n";
            }
        }
        return out;
    }
}}}
