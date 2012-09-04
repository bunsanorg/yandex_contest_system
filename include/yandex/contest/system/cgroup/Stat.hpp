#pragma once

#include "yandex/contest/system/cgroup/SubsystemBase.hpp"

#include <string>
#include <unordered_map>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    template <typename Config, typename T>
    class Stat: public SubsystemBase<Config>
    {
    public:
        std::unordered_map<std::string, T> stat()
        {
            const std::string stat_ = this->template readField<std::string>("stat");
            // TODO
        }
    };
}}}}
