#pragma once

#include "yandex/contest/system/cgroup/ResourceCounter.hpp"
#include "yandex/contest/system/cgroup/Stat.hpp"

#include "yandex/contest/StreamEnum.hpp"

#include <vector>
#include <chrono>

#include <boost/optional.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    class CpuAccountingBase:
        public virtual ResourceCounter<CpuAccountingBase, std::chrono::nanoseconds>,
        public virtual Stat<CpuAccountingBase, Count>
    {
    public:
        static const std::string SUBSYSTEM_NAME;
        static const boost::optional<std::string> UNITS;

    public:
        /// \warning Precision of TickDuration may be higher than system precision.
        typedef std::chrono::milliseconds TickDuration;

        typedef std::chrono::nanoseconds Duration;

    public:
        std::vector<Duration> usagePerCpu() const;

        TickDuration userUsage() const;
        TickDuration systemUsage() const;

    private:
        Count userUsageTicks() const;
        Count systemUsageTicks() const;

    private:
        static TickDuration ticksToDuration(const Count ticks);
        static Duration uintToDuration(const Count n);
    };

    typedef Subsystem<CpuAccountingBase> CpuAccounting;
}}}}
