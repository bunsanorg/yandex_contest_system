#pragma once

#include <yandex/contest/system/cgroup/SubsystemBase.hpp>
#include <yandex/contest/system/cgroup/Types.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    template <typename Config, typename Units_=Count,
              typename Converter=detail::UnitsConverter<Units_>>
    class ResourceCounter: public virtual SubsystemBase<Config>
    {
    public:
        typedef Units_ Units;

    public:
        Units usage() const
        {
            return Converter::countToUnits(
                this->template readField<Count>(fieldNameInUnits("usage")));
        }

        Units maxUsage() const
        {
            return Converter::countToUnits(
                this->template readField<Count>(fieldNameInUnits("max_usage")));
        }

        void resetMaxUsageToUsage() const
        {
            this->writeField(fieldNameInUnits("max_usage"), 0);
        }

    protected:
        static std::string fieldNameInUnits(const std::string &fieldName)
        {
            if (Config::UNITS)
                return fieldName + "_in_" + *Config::UNITS;
            else
                return fieldName;
        }
    };
}}}}
