#include "yandex/contest/system/cgroup/CpuAccounting.hpp"

#include <iterator>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    const std::string CpuAccountingBase::SUBSYSTEM_NAME = "cpuacct";
    const boost::optional<std::string> CpuAccountingBase::UNITS;

    std::vector<CpuAccountingBase::uint_t> CpuAccountingBase::usagePerCpu() const
    {
        std::vector<uint_t> usagePerCpu_;
        readFieldByReader("usage_percpu",
            [&usagePerCpu_](std::istream &in)
            {
                usagePerCpu_.assign(std::istream_iterator<uint_t>(in),
                                    std::istream_iterator<uint_t>());
            });
        return usagePerCpu_;
    }
}}}}
