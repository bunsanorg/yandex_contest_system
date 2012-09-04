#pragma once

#include "yandex/contest/system/cgroup/ResourceCounter.hpp"

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    template <typename Config>
    class ResourceLimiter: public ResourceCounter<Config>
    {
    public:
        typedef typename ResourceCounter<Config>::uint_t uint_t;

    public:
        uint_t limit() const
        {
            return this->template readField<uint_t>(
                ResourceCounter<Config>::fieldNameInUnits("limit"));
        }

        void setLimit(const uint_t limit) const
        {
            this->template writeField(
                ResourceCounter<Config>::fieldNameInUnits("limit"), limit);
            // TODO validate? kernel may not set it properly
        }

        uint_t failcnt() const
        {
            return this->template readField<uint_t>("failcnt");
        }

        void resetFailcnt() const
        {
            this->template writeField("failcnt", 0);
        }
    };
}}}}
