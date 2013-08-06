#pragma once

#include <yandex/contest/system/cgroup/ResourceCounter.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    template <typename Config, typename Units_=Count,
              typename Converter=detail::UnitsConverter<Units_>>
    class ResourceLimiter: public virtual ResourceCounter<Config, Units_, Converter>
    {
    public:
        typedef typename ResourceCounter<Config>::Units Units;

    public:
        Units limit() const
        {
            return Converter::countToUnits(this->template readField<Count>(
                ResourceCounter<Config>::fieldNameInUnits("limit")));
        }

        void setLimit(const Units limit) const
        {
            const Count limit_ = Converter::unitsToCount(limit);
            this->template writeField<Units>(
                ResourceCounter<Config>::fieldNameInUnits("limit"), limit_);
            // TODO validate? kernel may not set it properly
        }

        Count failcnt() const
        {
            return this->template readField<Count>("failcnt");
        }

        void resetFailcnt() const
        {
            this->template writeField("failcnt", 0);
        }
    };
}}}}
