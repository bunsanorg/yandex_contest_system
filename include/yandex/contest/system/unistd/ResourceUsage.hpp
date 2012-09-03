#pragma once

#include <cstdint>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

struct rusage;

namespace yandex{namespace contest{namespace system{namespace unistd
{
    struct ResourceUsage
    {
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar & BOOST_SERIALIZATION_NVP(timeUsageMillis);
            ar & BOOST_SERIALIZATION_NVP(memoryUsageBytes);
        }

        explicit ResourceUsage(const ::rusage &rusage);

        ResourceUsage()=default;
        ResourceUsage(const ResourceUsage &)=default;
        ResourceUsage &operator=(const ResourceUsage &)=default;

        void assign(const ::rusage &rusage);

        std::uint64_t timeUsageMillis = 0;
        std::uint64_t memoryUsageBytes = 0;
    };
}}}}
