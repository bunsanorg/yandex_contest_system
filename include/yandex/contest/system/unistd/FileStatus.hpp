#pragma once

#include "yandex/contest/system/unistd/access/Id.hpp"

#include <sys/types.h>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    struct FileStatus
    {
        dev_t      dev;     ///< ID of device containing file
        ino_t      ino;     ///< inode number
        mode_t     mode;    ///< protection
        nlink_t    nlink;   ///< number of hard links
        access::Id ownerId; ///< uid/gid of owner
        dev_t      rdev;    ///< device ID (if special file)
        off_t      size;    ///< total size, in bytes
        blksize_t  blksize; ///< blocksize for file system I/O
        blkcnt_t   blocks;  ///< number of 512B blocks allocated
        time_t     atime;   ///< time of last access
        time_t     mtime;   ///< time of last modification
        time_t     ctime;   ///< time of last status change

        /*!
         * \brief File access mode.
         *
         * \return mode & O_ACCMODE
         */
        mode_t accmode() const noexcept;

        /*!
         * \brief File type.
         *
         * \return mode & S_IFMT
         */
        mode_t type() const noexcept;

        bool isFifo() const noexcept;
        bool isCharDevice() const noexcept;
        bool isDirectory() const noexcept;
        bool isBlockDevice() const noexcept;
        bool isRegularFile() const noexcept;
        bool isSymbolicLink() const noexcept;
        bool isSocket() const noexcept;

        mode_t permissions() const noexcept;
    };
}}}}
