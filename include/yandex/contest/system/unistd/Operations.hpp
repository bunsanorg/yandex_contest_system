#pragma once

#include "yandex/contest/system/unistd/access/Id.hpp"

#include "yandex/contest/system/unistd/FileStatus.hpp"
#include "yandex/contest/system/unistd/Descriptor.hpp"

#include <system_error>

#include <boost/filesystem/path.hpp>

#include <sys/types.h>

struct rlimit;
struct itimerval;

namespace yandex{namespace contest{namespace system{namespace unistd
{
    /// Information for exceptions.
    namespace info
    {
        typedef boost::error_info<struct pathTag, boost::filesystem::path> path;
        typedef boost::error_info<struct modeTag, mode_t> mode;
        typedef boost::error_info<struct accessIdTag, access::Id> accessId;
        typedef boost::error_info<struct devMajorTag, int> devMajor;
        typedef boost::error_info<struct devMinorTag, int> devMinor;
        typedef boost::error_info<struct symLinkValueTag, boost::filesystem::path> symLinkValue;
        typedef boost::error_info<struct uidTag, uid_t> uid;
        typedef boost::error_info<struct ruidTag, uid_t> ruid;
        typedef boost::error_info<struct euidTag, uid_t> euid;
        typedef boost::error_info<struct suidTag, uid_t> suid;
        typedef boost::error_info<struct gidTag, gid_t> gid;
        typedef boost::error_info<struct rgidTag, gid_t> rgid;
        typedef boost::error_info<struct egidTag, gid_t> egid;
        typedef boost::error_info<struct sgidTag, gid_t> sgid;
        typedef boost::error_info<struct openFlagsTag, int> openFlags;
        typedef boost::error_info<struct fdTag, int> fd;
        typedef boost::error_info<struct oldFdTag, int> oldfd;
        typedef boost::error_info<struct newFdTag, int> newfd;
        typedef boost::error_info<struct resourceTag, int> resource;
        // TODO rlimit (how to do it without <sys/resource.h> include?
        // TODO interval timer (the same...)
        typedef boost::error_info<struct pidTag, pid_t> pid;
        typedef boost::error_info<struct signalTag, int> signal;
    }

    /// chmod(3)
    void chmod(const boost::filesystem::path &path, const mode_t mode);

    /// chown(3)
    void chown(const boost::filesystem::path &path, const access::Id &id);

    /// lchown(3)
    void lchown(const boost::filesystem::path &path, const access::Id &id);

    /// mkdir(3)
    void mkdir(const boost::filesystem::path &path, const mode_t mode);

    /// mknod(3)
    void mknod(const boost::filesystem::path &path, const mode_t mode, const dev_t dev);

    /// makedev(3)
    dev_t makedev(const int major, const int minor);

    /// symlink(3)
    void symlink(const boost::filesystem::path &value, const boost::filesystem::path &path);

    /// stat(3)
    FileStatus stat(const boost::filesystem::path &path);

    /// fstat(3)
    FileStatus fstat(int fd);

    /// lstat(3)
    FileStatus lstat(const boost::filesystem::path &path);

    /// getuid(3)
    uid_t getuid() noexcept;

    /// getgid(3)
    gid_t getgid() noexcept;

    /// geteuid(3)
    uid_t geteuid() noexcept;

    /// getegid(3)
    gid_t getegid() noexcept;

    /// getresuid(2)
    void getresuid(uid_t &ruid, uid_t &euid, uid_t &suid) noexcept;

    /// getresgid(2)
    void getresgid(gid_t &rgid, gid_t &egid, gid_t &sgid) noexcept;

    /// setuid(3)
    void setuid(const uid_t uid);

    /// setgid(3)
    void setgid(const gid_t gid);

    /// setreuid(3)
    void setreuid(const uid_t ruid, const uid_t euid);

    /// setregid(3)
    void setregid(const gid_t rgid, const gid_t egid);

    /// setresuid(2)
    void setresuid(const uid_t ruid, const uid_t euid, const uid_t suid);

    /// setresgid(2)
    void setresgid(const gid_t rgid, const gid_t egid, const gid_t sgid);

    /// fork(3)
    pid_t fork();

    /// open(3)
    Descriptor open(const boost::filesystem::path &path, const int oflag, const mode_t mode=0666);

    /// close(3)
    void close(const int fd);

    /// dup(3)
    Descriptor dup(const int fd);

    /*!
     * \brief dup2(3)
     *
     * \code{.cpp}
     * dup2(1, 2); // redirect stderr to stdout
     * \endcode
     */
    void dup2(const int oldfd, const int newfd);

    /// getdtablesize(3)
    unsigned getdtablesize();

    /// getrlimit(3)
    void getrlimit(const int resource, struct rlimit &rlp);

    /// setrlimit(3)
    void setrlimit(const int resource, const struct rlimit &rlp);

    /// getitimer(3)
    void getitimer(const int which, ::itimerval &curr_value);

    /// setitimer(3)
    void setitimer(const int which, const ::itimerval &new_value);

    /// setitimer(3)
    void setitimer(const int which, const ::itimerval &new_value, ::itimerval &old_value);

    /// kill(3)
    void kill(const pid_t pid, const int sig);

    /*!
     * \brief kill(pid, 0)
     *
     * \return error if happened.
     */
    std::error_code kill0(const pid_t pid) noexcept;
}}}}
