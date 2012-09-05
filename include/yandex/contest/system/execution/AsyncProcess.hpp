#pragma once

#include "yandex/contest/system/execution/CommonTypedefs.hpp"
#include "yandex/contest/system/execution/Result.hpp"

#include "yandex/contest/Tempfile.hpp"

#include <boost/filesystem/path.hpp>

#include <sys/types.h>

namespace yandex{namespace contest{namespace system{namespace execution
{
    class AsyncProcess
    {
    public:
        typedef ::pid_t Pid;

        struct Options
        {
            boost::filesystem::path executable;
            ProcessArguments arguments;
            std::string in; ///< Will be used as data for stdin.
            bool usePath = true;
        };

    public:
        /// Invalid AsyncProcess instance.
        AsyncProcess()=default;

        explicit AsyncProcess(const Options &options);
        AsyncProcess(const AsyncProcess &)=delete;
        AsyncProcess(AsyncProcess &&);
        AsyncProcess &operator=(const AsyncProcess &)=delete;
        AsyncProcess &operator=(AsyncProcess &&);

        /// If AsyncProcess is valid.
        explicit operator bool() const noexcept;

        /// Calls stop().
        ~AsyncProcess();

        /*!
         * \brief Wait for process termination.
         *
         * Set execution result.
         */
        const Result &wait();

        /*!
         * \brief Check if process has terminated.
         *
         * Set execution result if terminated.
         *
         * \return Initialized execution result if process
         * has terminated.
         */
        const boost::optional<Result> &poll();

        /// If process has not terminated try to kill and wait.
        void stop();

        /*!
         * \brief Get process id.
         *
         * \warning Value is unspecified if !(*this).
         */
        Pid pid() const noexcept;

        void swap(AsyncProcess &process) noexcept;

    private:
        void collectOutput();

    private:
        Tempfile in_, out_, err_;
        Pid pid_ = 0;
        boost::optional<Result> result_;
    };

    inline void swap(AsyncProcess &a, AsyncProcess &b) noexcept
    {
        a.swap(b);
    }
}}}}
