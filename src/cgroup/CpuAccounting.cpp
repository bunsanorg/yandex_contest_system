#include <yandex/contest/system/cgroup/CpuAccounting.hpp>

#include <yandex/contest/system/unistd/Operations.hpp>

#include <boost/iterator/transform_iterator.hpp>

#include <iterator>

#include <unistd.h>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {

namespace cpu_accounting_detail {
TickDuration TickDurationConverter::countToUnits(const Count ticks) {
  const long clkTck = unistd::sysconf(_SC_CLK_TCK);
  constexpr Count den = TickDuration::period::den;
  const Count num = TickDuration::period::num * clkTck;
  BOOST_ASSERT_MSG(num <= den, "Precision of TickDuration is too low.");
  return TickDuration((ticks * den) / num);
}

// Count TickDurationConverter::unitsToCount(const TickDuration units);
}  // namespace cpu_accounting_detail

const std::string CpuAccountingBase::SUBSYSTEM_NAME = "cpuacct";
const boost::optional<std::string> CpuAccountingBase::UNITS;

std::vector<CpuAccountingBase::Duration> CpuAccountingBase::usagePerCpu()
    const {
  std::vector<Duration> usagePerCpu_;
  readFieldByReader("usage_percpu", [&usagePerCpu_](std::istream &in) {
    const auto begin = boost::make_transform_iterator(
        std::istream_iterator<Count>(in), uintToDuration);
    const auto end = boost::make_transform_iterator(
        std::istream_iterator<Count>(), uintToDuration);
    usagePerCpu_.assign(begin, end);
  });
  return usagePerCpu_;
}

CpuAccountingBase::TickDuration CpuAccountingBase::userUsage() const {
  return stat().userUsage;
}

CpuAccountingBase::TickDuration CpuAccountingBase::systemUsage() const {
  return stat().systemUsage;
}

CpuAccountingBase::Duration CpuAccountingBase::uintToDuration(const Count n) {
  return Duration(n);
}

}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
