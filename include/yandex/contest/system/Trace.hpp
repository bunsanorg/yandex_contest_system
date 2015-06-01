#pragma once

#include <boost/filesystem/path.hpp>

#include <csignal>

namespace yandex{namespace contest{namespace system
{
    class Trace: public std::vector<void *>
    {
    public:
        Trace()=default;
        Trace(const Trace &)=default;
        Trace &operator=(const Trace &)=default;
        Trace(Trace &&)=default;
        Trace &operator=(Trace &&)=default;

    public:
        static Trace get(const std::size_t maxSize=1024);

        static void handler(int sig, siginfo_t *siginfo, void *context);

        static void handle(const int sig, void (*h)(int, siginfo_t *, void *));

        static void handle(const int sig)
        {
            handle(sig, &handler);
        }

    private:
        friend std::ostream &operator<<(std::ostream &out, const Trace &trace);
    };

    std::ostream &operator<<(std::ostream &out, const Trace &trace);
}}}
