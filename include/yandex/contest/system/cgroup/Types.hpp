#pragma once

#include <cstdint>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    typedef std::uint64_t Count;

    namespace detail
    {
        template <typename Units>
        struct UnitsConverter
        {
            static Count unitsToCount(const Units units)
            {
                return static_cast<Count>(units);
            }

            static Units countToUnits(const Count count)
            {
                return static_cast<Units>(count);
            }
        };
    }
}}}}
