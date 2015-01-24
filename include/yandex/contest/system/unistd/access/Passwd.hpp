#pragma once

#include <yandex/contest/system/unistd/access/Id.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

struct passwd;

namespace yandex{namespace contest{namespace system{
    namespace unistd{namespace access
{
    struct Passwd
    {
        std::string name;
        std::string passwd;
        Id id;
        std::string gecos;
        boost::filesystem::path dir;
        std::string shell;

        static Passwd load(const ::passwd &pw);
    };
}}}}}
