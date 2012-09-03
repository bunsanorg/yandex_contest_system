#pragma once

#include "yandex/contest/system/unistd/MountEntry.hpp"

#include <vector>

#include <boost/filesystem/path.hpp>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    struct Fstab
    {
        std::vector<MountEntry> entries;

        /// Load from file with fstab(5) syntax.
        void load(const boost::filesystem::path &fstab);

        /// Save to file with fstab(5) syntax.
        void save(const boost::filesystem::path &fstab) const;
    };
}}}}
