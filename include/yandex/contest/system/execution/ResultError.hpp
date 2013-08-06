#pragma once

#include <yandex/contest/system/Error.hpp>

#include <yandex/contest/system/execution/Result.hpp>

namespace yandex{namespace contest{namespace system{namespace execution
{
    struct ResultError: virtual Error
    {
        ResultError()=default;
        explicit ResultError(const Result &result);

        typedef boost::error_info<struct termSigtag, int> termSig;
        typedef boost::error_info<struct exitStatusTag, int> exitStatus;
        typedef boost::error_info<struct outTag, std::string> out;
        typedef boost::error_info<struct errTag, std::string> err;
    };
}}}}
