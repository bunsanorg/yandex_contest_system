#pragma once

#include <boost/optional.hpp>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    struct ProcessResult
    {
        explicit ProcessResult(const int statLoc);
        ProcessResult()=default;
        ProcessResult(const ProcessResult &)=default;
        ProcessResult &operator=(const ProcessResult &)=default;

        void assign(const int statLoc);

        /// If completed successfully.
        explicit operator bool() const;

        /*!
         * \brief If process terminated normally,
         * field is initialized by process exit status.
         *
         * See wait(3), WEXITSTATUS, WIFEXITED.
         */
        boost::optional<int> exitStatus;

        /*!
         * \brief If child process was terminated due to the receipt of a signal
         * that was not caught, the field is initialized by the signal number.
         *
         * See wait(3), WTERMSIG, WIFSIGNALED.
         */
        boost::optional<int> termSig;
    };
}}}}
