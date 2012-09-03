#pragma once

#include "yandex/contest/system/unistd/Error.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    struct MountEntryFormatError: virtual Error {};

    struct MountEntryUninitializedFieldError: virtual MountEntryFormatError
    {
        typedef boost::error_info<struct fieldTag, std::string> field;
    };

    struct MountEntryInvalidRepresentationError: virtual MountEntryFormatError
    {
        typedef boost::error_info<struct lineTag, std::string> line;
    };

    struct MountEntry
    {
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar & BOOST_SERIALIZATION_NVP(fsname);
            ar & BOOST_SERIALIZATION_NVP(dir);
            ar & BOOST_SERIALIZATION_NVP(type);
            ar & BOOST_SERIALIZATION_NVP(opts);
            ar & BOOST_SERIALIZATION_NVP(freq);
            ar & BOOST_SERIALIZATION_NVP(passno);
        }

        MountEntry()=default;

        /// Load from fstab line representation.
        explicit MountEntry(const std::string &line);

        /// Convert to fstab line representation.
        explicit operator std::string() const;

        MountEntry(const MountEntry &)=default;
        MountEntry &operator=(const MountEntry &)=default;

        /// Device or server for filesystem.
        std::string fsname;

        /// Directory mounted on.
        std::string dir;

        /// Type of filesystem: ufs, nfs, etc.
        std::string type;

        /// Mount options.
        std::string opts;

        /// Dump frequency (in days).
        int freq = 0;

        /// Pass number for `fsck'.
        int passno = 0;

        /// Replace some space characters by octal codes.
        static std::string escape(const std::string &field);

        /// Replace octal codes by characters.
        static std::string unescape(const std::string &field);

        static MountEntry bind(const boost::filesystem::path &from,
                               const boost::filesystem::path &to,
                               const std::string &opts=std::string());

        static MountEntry bindRO(const boost::filesystem::path &from,
                                 const boost::filesystem::path &to);

        static MountEntry proc();
    };

    std::ostream &operator<<(std::ostream &out, const MountEntry &entry);
}}}}
