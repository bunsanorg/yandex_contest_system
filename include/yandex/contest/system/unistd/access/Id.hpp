#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <iostream>

#include <sys/types.h>

namespace yandex{namespace contest{namespace system{
    namespace unistd{namespace access
{
    struct Id
    {
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar & BOOST_SERIALIZATION_NVP(uid);
            ar & BOOST_SERIALIZATION_NVP(gid);
        }

        Id()=default;
        Id(const uid_t uid_, const gid_t gid_): uid(uid_), gid(gid_) {}
        Id(const Id &)=default;
        Id &operator=(const Id &)=default;

        bool operator==(const Id &id) const noexcept;

        uid_t uid = 0;
        gid_t gid = 0;
    };

    std::ostream &operator<<(std::ostream &out, const Id &id);
}}}}}
