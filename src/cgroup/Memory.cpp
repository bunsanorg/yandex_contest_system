#include "yandex/contest/system/cgroup/Memory.hpp"

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    const std::string MemoryBase::SUBSYSTEM_NAME("memory");
    const boost::optional<std::string> MemoryBase::UNITS("bytes");
}}}}
