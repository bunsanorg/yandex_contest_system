#pragma once

#include "yandex/contest/system/cgroup/SubsystemBase.hpp"

#include <string>

#include <cstdint>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    template <typename Config>
    class ResourceCounter: public SubsystemBase<Config>
    {
    public:
        typedef std::uint64_t uint_t;

    public:
        uint_t usage() const
        {
            return this->template readField<uint_t>(fieldNameInUnits("usage"));
        }

        uint_t maxUsage() const
        {
            return this->template readField<uint_t>(fieldNameInUnits("max_usage"));
        }

        void resetMaxUsageToUsage() const
        {
            this->writeField(fieldNameInUnits("max_usage"), 0);
        }

    protected:
        static inline std::string fieldNameInUnits(const std::string &fieldName)
        {
            if (Config::UNITS)
                return fieldName + "_in_" + *Config::UNITS;
            else
                return fieldName;
        }
    };
}}}}
