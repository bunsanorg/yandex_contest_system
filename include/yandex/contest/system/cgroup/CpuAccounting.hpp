#pragma once

#include "yandex/contest/system/cgroup/ResourceCounter.hpp"
#include "yandex/contest/system/cgroup/Stat.hpp"

#include "yandex/contest/StreamEnum.hpp"

#include <vector>

#include <boost/optional.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    class CpuAccountingBase:
        public virtual ResourceCounter<CpuAccountingBase>,
        public virtual Stat<CpuAccountingBase, ResourceCounter<CpuAccountingBase>::uint_t>
    {
    public:
        static const std::string SUBSYSTEM_NAME;
        static const boost::optional<std::string> UNITS;

    public:
        typedef typename ResourceCounter<CpuAccountingBase>::uint_t uint_t;

    public:
        std::vector<uint_t> usagePerCpu() const;
    };

    typedef Subsystem<CpuAccountingBase> CpuAccounting;
}}}}
