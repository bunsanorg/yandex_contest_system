#pragma once

#include "yandex/contest/system/cgroup/ResourceLimiter.hpp"
#include "yandex/contest/system/cgroup/Stat.hpp"

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    class MemorySwapBase:
        public virtual ResourceLimiter<MemorySwapBase>,
        public virtual Stat<MemorySwapBase, ResourceCounter<MemorySwapBase>::uint_t>
    {
    public:
        static const std::string SUBSYSTEM_NAME;
        static const boost::optional<std::string> UNITS;
    };

    typedef Subsystem<MemorySwapBase> MemorySwap;
}}}}
