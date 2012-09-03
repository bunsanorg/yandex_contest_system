#pragma once

#include "yandex/contest/system/Error.hpp"

#include "yandex/contest/system/execution/ResultError.hpp"

#include "yandex/contest/system/lxc/State.hpp"

#include <utility>
#include <string>

namespace yandex{namespace contest{namespace system{namespace lxc
{
    struct Error: virtual system::Error
    {
        typedef boost::error_info<struct nameTag, std::string> name;
    };

    struct IllegalStateError: virtual Error
    {
        typedef boost::error_info<struct stateTag, lxc_detail::State> state;
    };

    struct UtilityError:
        virtual Error,
        virtual execution::ResultError
    {
        template <typename ... Args>
        explicit UtilityError(Args &&...args):
            execution::ResultError(std::forward<Args>(args)...) {}
    };

    struct ConfigError: virtual Error
    {
        typedef boost::error_info<struct keyTag, std::string> key;
        typedef boost::error_info<struct lineTag, std::string> line;
    };
}}}}
