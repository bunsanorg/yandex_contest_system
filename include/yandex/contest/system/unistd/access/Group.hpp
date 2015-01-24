#pragma once

#include <yandex/contest/system/unistd/access/Id.hpp>

#include <string>
#include <vector>

struct group;

namespace yandex{namespace contest{namespace system{
    namespace unistd{namespace access
{
    struct Group
    {
        std::string name;
        std::string passwd;
        gid_t gid;
        std::vector<std::string> members;

        static Group load(const ::group &gr);
    };
}}}}}
