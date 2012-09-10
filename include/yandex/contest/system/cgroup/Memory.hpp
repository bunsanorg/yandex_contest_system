#pragma once

#include "yandex/contest/system/cgroup/ResourceLimiter.hpp"
#include "yandex/contest/system/cgroup/Stat.hpp"

#include <boost/optional.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    class MemoryBase:
        public virtual ResourceLimiter<MemoryBase, Count>,
        public virtual Stat<MemoryBase, Count>
    {
    public:
        static const std::string SUBSYSTEM_NAME;
        static const boost::optional<std::string> UNITS;

    public:
        struct MoveChargeAtImmigrateConfig
        {
            MoveChargeAtImmigrateConfig()=default;

            explicit MoveChargeAtImmigrateConfig(const int mask);

            int mask() const;

            bool anonymous = false;
            bool file = false;
        };

        MoveChargeAtImmigrateConfig moveChargeAtImmigrate() const;
        void setMoveChargeAtImmigrate(
            const MoveChargeAtImmigrateConfig &moveChargeAtImmigrate) const;
        void setMoveChargeAtImmigrate(
            const bool anonymous=true, const bool file=true) const;

        Count softLimitInBytes() const;
        void setSoftLimitInBytes(const Count limit) const;

        bool underOom() const;
        bool oomKillDisable() const;
        void setOomKillDisable(const bool oomControl=true) const;

        void forceEmpty() const;
    };

    typedef Subsystem<MemoryBase> Memory;
}}}}
