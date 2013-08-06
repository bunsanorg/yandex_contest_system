#pragma once

#include <yandex/contest/system/cgroup/SubsystemBase.hpp>
#include <yandex/contest/system/cgroup/Types.hpp>

#include <string>
#include <unordered_map>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    template <typename Config, typename Units_=Count,
              typename Converter=detail::UnitsConverter<Units_>>
    class Stat: public virtual SubsystemBase<Config>
    {
    public:
        typedef Units_ Units;
        typedef std::unordered_map<std::string, Units> Map;

    public:
        Map stat() const
        {
            Map map;
            this->readFieldByReader("stat",
                [&map](std::istream &in)
                {
                    std::string key;
                    Count value;
                    while (in >> key >> value)
                        map.emplace(key, Converter::countToUnits(value));
                });
            return map;
        }
    };
}}}}
