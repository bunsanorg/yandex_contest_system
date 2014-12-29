#pragma once

#include <yandex/contest/system/execution/CommonTypedefs.hpp>

#include <boost/filesystem/path.hpp>

#include <utility>

namespace yandex{namespace contest{namespace system{namespace execution
{
    struct CollectHelper
    {
        static inline void collectTo(ProcessArguments &/*arguments*/)
        {
            /*does nothing*/
        }

        static inline const std::string &toString(const std::string &arg)
        {
            return arg;
        }

        static inline std::string toString(const boost::filesystem::path &arg)
        {
            return arg.string();
        }

        static inline std::string toString(const char *const arg)
        {
            return arg;
        }

        template <typename T, typename ... Args>
        static inline void collectTo(ProcessArguments &arguments,
                                     const T &arg, Args &&...args)
        {
            arguments.push_back(toString(arg));
            collectTo(arguments, std::forward<Args>(args)...);
        }

        template <typename ... Args>
        static inline void collectTo(
            ProcessArguments &arguments,
            const std::vector<std::string> &arg, Args &&...args)
        {
            arguments.insert(arguments.end(), arg.begin(), arg.end());
            collectTo(arguments, std::forward<Args>(args)...);
        }
    };

    template <typename ... Args>
    static inline ProcessArguments collect(Args &&...args)
    {
        ProcessArguments arguments;
        CollectHelper::collectTo(arguments, std::forward<Args>(args)...);
        return arguments;
    }
}}}}
