#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace yandex{namespace contest{namespace system{namespace execution
{
    typedef std::vector<std::string> ProcessArguments;
    typedef std::unordered_map<std::string, std::string> ProcessEnvironment;
}}}}
