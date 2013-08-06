#pragma once

#include <yandex/contest/system/cgroup/ResourceCounter.hpp>
#include <yandex/contest/system/cgroup/StructStat.hpp>

#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <chrono>
#include <vector>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    namespace cpu_accounting_detail
    {
        /// \warning Precision of TickDuration may be higher than system precision.
        typedef std::chrono::duration<Count, std::centi> TickDuration;

        struct CpuAccountingStat
        {
            template <typename Archive>
            void serialize(Archive &ar, const unsigned int)
            {
                ar & boost::serialization::make_nvp("user", userUsage);
                ar & boost::serialization::make_nvp("system", systemUsage);
            }

            TickDuration userUsage;
            TickDuration systemUsage;
        };

        struct TickDurationConverter
        {
            static TickDuration countToUnits(const Count count);
            static Count unitsToCount(const TickDuration units);
        };
    }

    class CpuAccountingBase:
        public virtual ResourceCounter<CpuAccountingBase, std::chrono::nanoseconds>,
        public virtual StructStat<CpuAccountingBase,
                                  cpu_accounting_detail::TickDuration,
                                  cpu_accounting_detail::CpuAccountingStat,
                                  cpu_accounting_detail::TickDurationConverter>
    {
    public:
        static const std::string SUBSYSTEM_NAME;
        static const boost::optional<std::string> UNITS;

    public:
        typedef std::chrono::nanoseconds Duration;
        typedef cpu_accounting_detail::TickDuration TickDuration;
        typedef cpu_accounting_detail::CpuAccountingStat Stat;

    public:
        std::vector<Duration> usagePerCpu() const;

        TickDuration userUsage() const;
        TickDuration systemUsage() const;

    private:
        static Duration uintToDuration(const Count n);
    };

    typedef Subsystem<CpuAccountingBase> CpuAccounting;
}}}}
