#include "yandex/contest/system/cgroup/Memory.hpp"

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    const std::string MemoryBase::SUBSYSTEM_NAME("memory");
    const boost::optional<std::string> MemoryBase::UNITS("bytes");

    MemoryBase::MoveChargeAtImmigrateConfig::MoveChargeAtImmigrateConfig(const int mask):
        anonymous(mask & (1 << 0)), file(mask & (1 << 1)) {}

    int MemoryBase::MoveChargeAtImmigrateConfig::mask() const
    {
        return (static_cast<int>(anonymous) << 0) | (static_cast<int>(file) << 1);
    }

    MemoryBase::MoveChargeAtImmigrateConfig MemoryBase::moveChargeAtImmigrate() const
    {
        return MoveChargeAtImmigrateConfig(readField<int>("move_charge_at_immigrate"));
    }

    void MemoryBase::setMoveChargeAtImmigrate(
        const MoveChargeAtImmigrateConfig &moveChargeAtImmigrate) const
    {
        writeField("move_charge_at_immigrate", moveChargeAtImmigrate.mask());
    }

    void MemoryBase::setMoveChargeAtImmigrate(const bool anonymous, const bool file) const
    {
        MoveChargeAtImmigrateConfig cfg;
        cfg.anonymous = anonymous;
        cfg.file = file;
        setMoveChargeAtImmigrate(cfg);
    }

    Count MemoryBase::softLimitInBytes() const
    {
        return readField<Count>("soft_limit_in_bytes");
    }

    void MemoryBase::setSoftLimitInBytes(const Count limit) const
    {
        writeField("soft_limit_in_bytes", limit);
    }

    bool MemoryBase::oomControl() const
    {
        return readField<int>("oom_control");
    }

    void MemoryBase::setOomControl(const bool oomControl) const
    {
        writeField<int>("oom_control", oomControl);
    }

    void MemoryBase::forceEmpty() const
    {
        writeField("force_empty", 0);
    }
}}}}
