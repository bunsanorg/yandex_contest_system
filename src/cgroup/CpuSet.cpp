#include <yandex/contest/system/cgroup/CpuSet.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    const std::string CpuSetBase::SUBSYSTEM_NAME("cpuset");

    std::string CpuSetBase::cpus() const
    {
        return readField<std::string>("cpus");
    }

    void CpuSetBase::setCpus(const std::string &cpus) const
    {
        writeField("cpus", cpus);
    }

    std::string CpuSetBase::mems() const
    {
        return readField<std::string>("mems");
    }

    void CpuSetBase::setMems(const std::string &mems) const
    {
        writeField("mems", mems);
    }

    bool CpuSetBase::memoryMigrate() const
    {
        return readField<int>("memory_migrate");
    }

    void CpuSetBase::setMemoryMigrate(const bool memoryMigrate) const
    {
        writeField<int>("memory_migrate", memoryMigrate);
    }
}}}}
