#include <yandex/contest/system/unistd/FileStatus.hpp>

#include <fcntl.h>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    mode_t FileStatus::accmode() const noexcept
    {
        return mode & O_ACCMODE;
    }

    mode_t FileStatus::type() const noexcept
    {
        return mode & S_IFMT;
    }

    bool FileStatus::isFifo() const noexcept
    {
        return mode & S_IFIFO;
    }

    bool FileStatus::isCharDevice() const noexcept
    {
        return mode & S_IFCHR;
    }

    bool FileStatus::isDirectory() const noexcept
    {
        return mode & S_IFDIR;
    }

    bool FileStatus::isBlockDevice() const noexcept
    {
        return mode & S_IFBLK;
    }

    bool FileStatus::isRegularFile() const noexcept
    {
        return mode & S_IFREG;
    }

    bool FileStatus::isSymbolicLink() const noexcept
    {
        return mode & S_IFLNK;
    }

    bool FileStatus::isSocket() const noexcept
    {
        return mode & S_IFSOCK;
    }

    mode_t FileStatus::permissions() const noexcept
    {
        return mode & (S_IRWXU | S_IRWXG | S_IRWXO);
    }
}}}}
