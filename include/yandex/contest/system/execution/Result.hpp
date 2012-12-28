#pragma once

#include "yandex/contest/system/unistd/ProcessResult.hpp"

#include "bunsan/forward_constructor.hpp"

#include <string>

namespace yandex{namespace contest{namespace system{namespace execution
{
    struct Result: unistd::ProcessResult
    {
        BUNSAN_INHERIT_EXPLICIT_CONSTRUCTOR(Result, unistd::ProcessResult)

        Result()=default;
        Result(const Result &)=default;
        Result &operator=(const Result &)=default;

        std::string out;
        std::string err;
    };
}}}}
