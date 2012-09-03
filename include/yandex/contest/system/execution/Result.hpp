#pragma once

#include "yandex/contest/system/unistd/ProcessResult.hpp"

#include <utility>
#include <string>

namespace yandex{namespace contest{namespace system{namespace execution
{
    struct Result: unistd::ProcessResult
    {
        template <typename ... Args>
        explicit Result(Args &&...args):
            unistd::ProcessResult(std::forward<Args>(args)...) {}
        Result()=default;
        Result(const Result &)=default;
        Result &operator=(const Result &)=default;

        std::string out;
        std::string err;
    };
}}}}
