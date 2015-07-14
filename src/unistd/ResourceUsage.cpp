#include <yandex/contest/system/unistd/ResourceUsage.hpp>

#include <sys/resource.h>

namespace yandex {
namespace contest {
namespace system {
namespace unistd {

ResourceUsage::ResourceUsage(const ::rusage &rusage) { assign(rusage); }

void ResourceUsage::assign(const ::rusage &rusage) {
  memoryUsageBytes = rusage.ru_maxrss * 1024;
  // TODO consider correct roundness
  timeUsageMillis =
      rusage.ru_utime.tv_sec * 1000 + rusage.ru_utime.tv_usec / 1000;
}

}  // namespace unistd
}  // namespace system
}  // namespace contest
}  // namespace yandex
