#pragma once

#include "yandex/contest/system/cgroup/ResourceCounter.hpp"

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    template <typename Config, typename Units_=Count>
    class ResourceLimiter: public virtual ResourceCounter<Config, Units_>
    {
    public:
        typedef typename ResourceCounter<Config>::Units Units;

    public:
        Units limit() const
        {
            return Units(this->template readField<Count>(
                ResourceCounter<Config>::fieldNameInUnits("limit")));
        }

        // TODO support Units
        void setLimit(const Count limit) const
        {
            this->template writeField(
                ResourceCounter<Config>::fieldNameInUnits("limit"), limit);
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
