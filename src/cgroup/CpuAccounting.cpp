#include "yandex/contest/system/cgroup/CpuAccounting.hpp"

#include "yandex/contest/system/unistd/Operations.hpp"

#include <iterator>

#include <boost/iterator/transform_iterator.hpp>

#include <unistd.h>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    const std::string CpuAccountingBase::SUBSYSTEM_NAME = "cpuacct";
    const boost::optional<std::string> CpuAccountingBase::UNITS;

    std::vector<CpuAccountingBase::Duration> CpuAccountingBase::usagePerCpu() const
    {
        std::vector<Duration> usagePerCpu_;
        readFieldByReader("usage_percpu",
            [&usagePerCpu_](std::istream &in)
            {
                const auto begin = boost::make_transform_iterator(
                    std::istream_iterator<Count>(in), uintToDuration);
                const auto end = boost::make_transform_iterator(
                    std::istream_iterator<Count>(), uintToDuration);
                usagePerCpu_.assign(begin, end);
            });
        return usagePerCpu_;
    }

    CpuAccountingBase::TickDuration CpuAccountingBase::userUsage() const
    {
        return ticksToDuration(userUsageTicks());
    }

    CpuAccountingBase::TickDuration CpuAccountingBase::systemUsage() const
    {
        return ticksToDuration(systemUsageTicks());
    }

    Count CpuAccountingBase::userUsageTicks() const
    {
        const std::unordered_map<std::string, Count> stat_ = stat();
        BOOST_ASSERT(stat_.find("user") != stat_.end());
        return stat_.at("user");
    }

    Count CpuAccountingBase::systemUsageTicks() const
    {
        const std::unordered_map<std::string, Count> stat_ = stat();
        BOOST_ASSERT(stat_.find("system") != stat_.end());
        return stat_.at("system");
    }

    CpuAccountingBase::TickDuration CpuAccountingBase::ticksToDuration(const Count ticks)
    {
        const long clkTck = unistd::sysconf(_SC_CLK_TCK);
        constexpr Count den = TickDuration::period::den;
        const Count num = TickDuration::period::num * clkTck;
        BOOST_ASSERT_MSG(num <= den, "Precision of TickDuration is too low.");
        return TickDuration((ticks * den) / num);
    }

    CpuAccountingBase::Duration CpuAccountingBase::uintToDuration(const Count n)
    {
        return Duration(n);
    }
}}}}
