#include <yandex/contest/system/cgroup/MemorySwap.hpp>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {

const std::string MemorySwapBase::SUBSYSTEM_NAME("memory.memsw");
const boost::optional<std::string> MemorySwapBase::UNITS("bytes");

}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
