#include "yandex/contest/system/cgroup/CpuAccounting.hpp"

#include <algorithm>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    const std::string CpuAccountingBase::SUBSYSTEM_NAME = "cpuacct";
    const boost::optional<std::string> CpuAccountingBase::UNITS;

    std::vector<CpuAccountingBase::uint_t> CpuAccountingBase::usagePerCpu() const
    {
        std::vector<std::string> usagePerCpuStrings;
        std::string field = ResourceCounter<CpuAccountingBase>::
                                readField<std::string>("usage_percpu");
        boost::algorithm::split(usagePerCpuStrings, field,
                                boost::algorithm::is_space(),
                                boost::algorithm::token_compress_on);
        std::vector<uint_t> usagePerCpu_(usagePerCpuStrings.size());
        std::transform(usagePerCpuStrings.begin(), usagePerCpuStrings.end(),
                       usagePerCpu_.begin(), boost::lexical_cast<uint_t, std::string>);
        return usagePerCpu_;
    }
}}}}
