#pragma once

#include <yandex/contest/system/execution/Helper.hpp>
#include <yandex/contest/system/execution/Result.hpp>

namespace yandex{namespace contest{namespace system{namespace execution
{
    /*!
     * \brief Synchronized execution of process, Result will have Result::err filled.
     *
     * This implementation is the fastest (but has less features).
     * It is suitable for calling an utility
     * (Result::err can be used as error message in exception).
     */
    Result getErrCallImpl(const std::string &executable,
                          const ProcessArguments &arguments,
                          const bool usePath=true);

    /// \copydoc getErrCallImpl
    template <typename ... Args>
    Result getErrCall(const std::string &executable, Args &&...args, const bool usePath)
    {
        return getErrCallImpl(executable, collect(std::forward<Args>(args)...), usePath);
    }

    /// \copydoc getErrCallImpl
    template <typename ... Args>
    Result getErrCall(const std::string &executable, Args &&...args)
    {
        return getErrCallImpl(executable, collect(std::forward<Args>(args)...));
    }

    /// \copydoc getErrCallImpl
    template <typename ... Args>
    Result getErrCallArgv(const std::string &executable, Args &&...args)
    {
        return getErrCall(executable, executable, std::forward<Args>(args)...);
    }
}}}}
