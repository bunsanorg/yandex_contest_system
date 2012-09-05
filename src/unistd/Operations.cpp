#include "yandex/contest/system/unistd/Operations.hpp"

#include "yandex/contest/SystemError.hpp"

#include <csignal>

#include <boost/assert.hpp>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/time.h>

// internal defines

#define YANDEX_UNISTD_WRAP(X) if ((X) < 0) BOOST_THROW_EXCEPTION(SystemError(__func__))

#define YANDEX_UNISTD_RETURN(X) \
    const auto ret = X; \
    if (ret < 0) \
        BOOST_THROW_EXCEPTION(SystemError(__func__)); \
    else \
        return ret

#define YANDEX_UNISTD_ASSIGN(X, OBJ) \
    const auto ret = X; \
    if (ret < 0) \
        BOOST_THROW_EXCEPTION(SystemError(__func__)); \
    else \
        OBJ.assign(ret);

namespace yandex{namespace contest{namespace system{namespace unistd
{
    void chmod(const boost::filesystem::path &path, const mode_t mode)
    {
        YANDEX_UNISTD_WRAP(::chmod(path.c_str(), mode));
    }

    void chown(const boost::filesystem::path &path, const access::Id &id)
    {
        YANDEX_UNISTD_WRAP(::chown(path.c_str(), id.uid, id.gid));
    }

    void lchown(const boost::filesystem::path &path, const access::Id &id)
    {
        YANDEX_UNISTD_WRAP(::lchown(path.c_str(), id.uid, id.gid));
    }

    void mkdir(const boost::filesystem::path &path, const mode_t mode)
    {
        YANDEX_UNISTD_WRAP(::mkdir(path.c_str(), mode));
    }

    void mknod(const boost::filesystem::path &path, const mode_t mode, const dev_t dev)
    {
        YANDEX_UNISTD_WRAP(::mknod(path.c_str(), mode, dev));
    }

    dev_t makedev(const int major, const int minor)
    {
        return ::makedev(major, minor);
    }

    void symlink(const boost::filesystem::path &value, const boost::filesystem::path &path)
    {
        YANDEX_UNISTD_WRAP(::symlink(value.c_str(), path.c_str()));
    }

    namespace
    {
        struct StatusType: stat
        {
            operator FileStatus() const
            {
                return {
                    st_dev,
                    st_ino,
                    st_mode,
                    st_nlink,
                    {st_uid, st_gid},
                    st_rdev,
                    st_size,
                    st_blksize,
                    st_blocks,
                    st_atime,
                    st_mtime,
                    st_ctime
                };
            }
        };
    }

    FileStatus stat(const boost::filesystem::path &path)
    {
        StatusType buf;
        YANDEX_UNISTD_WRAP(::stat(path.c_str(), &buf));
        return buf;
    }

    FileStatus fstat(int fd)
    {
        StatusType buf;
        YANDEX_UNISTD_WRAP(::fstat(fd, &buf));
        return buf;
    }

    FileStatus lstat(const boost::filesystem::path &path)
    {
        StatusType buf;
        YANDEX_UNISTD_WRAP(::lstat(path.c_str(), &buf));
        return buf;
    }

    uid_t getuid() noexcept
    {
        return ::getuid();
    }

    gid_t getgid() noexcept
    {
        return ::getgid();
    }

    uid_t geteuid() noexcept
    {
        return ::geteuid();
    }

    gid_t getegid() noexcept
    {
        return ::getegid();
    }

    void getresuid(uid_t &ruid, uid_t &euid, uid_t &suid) noexcept
    {
        BOOST_VERIFY(::getresuid(&ruid, &euid, &suid) == 0);
    }

    void getresgid(gid_t &rgid, gid_t &egid, gid_t &sgid) noexcept
    {
        BOOST_VERIFY(::getresgid(&rgid, &egid, &sgid) == 0);
    }

    void setuid(const uid_t uid)
    {
        YANDEX_UNISTD_WRAP(::setuid(uid));
    }

    void setgid(const gid_t gid)
    {
        YANDEX_UNISTD_WRAP(::setgid(gid));
    }

    void setreuid(const uid_t ruid, const uid_t euid)
    {
        YANDEX_UNISTD_WRAP(::setreuid(ruid, euid));
    }

    void setregid(const gid_t rgid, const gid_t egid)
    {
        YANDEX_UNISTD_WRAP(::setregid(rgid, egid));
    }

    void setresuid(const uid_t ruid, const uid_t euid, const uid_t suid)
    {
        YANDEX_UNISTD_WRAP(::setresuid(ruid, euid, suid));
    }

    void setresgid(const gid_t rgid, const gid_t egid, const gid_t sgid)
    {
        YANDEX_UNISTD_WRAP(::setresgid(rgid, egid, sgid));
    }

    pid_t fork()
    {
        YANDEX_UNISTD_RETURN(::fork());
    }

    Descriptor open(const boost::filesystem::path &path, const int oflag, const mode_t mode)
    {
        Descriptor retfd;
        YANDEX_UNISTD_ASSIGN(::open(path.c_str(), oflag, mode), retfd);
        return retfd;
    }

    void close(const int fd)
    {
        YANDEX_UNISTD_WRAP(::close(fd));
    }

    Descriptor dup(const int fd)
    {
        Descriptor retfd;
        YANDEX_UNISTD_ASSIGN(::dup(fd), retfd);
        return retfd;
    }

    void dup2(const int oldfd, const int newfd)
    {
        int retfd;
        YANDEX_UNISTD_WRAP(retfd = ::dup2(oldfd, newfd));
        BOOST_ASSERT(newfd == retfd);
    }

    unsigned getdtablesize()
    {
        YANDEX_UNISTD_RETURN(::getdtablesize());
    }

    void getrlimit(const int resource, struct rlimit &rlp)
    {
        YANDEX_UNISTD_WRAP(::getrlimit(resource, &rlp));
    }

    void setrlimit(const int resource, const struct rlimit &rlp)
    {
        YANDEX_UNISTD_WRAP(::setrlimit(resource, &rlp));
    }

    void getitimer(const int which, ::itimerval &curr_value)
    {
        YANDEX_UNISTD_WRAP(::getitimer(which, &curr_value));
    }

    void setitimer(const int which, const ::itimerval &new_value)
    {
        YANDEX_UNISTD_WRAP(::setitimer(which, &new_value, nullptr));
    }

    void setitimer(const int which, const ::itimerval &new_value, ::itimerval &old_value)
    {
        YANDEX_UNISTD_WRAP(::setitimer(which, &new_value, &old_value));
    }

    void kill(const pid_t pid, const int sig)
    {
        YANDEX_UNISTD_WRAP(::kill(pid, sig));
    }

    std::error_code kill0(const pid_t pid) noexcept
    {
        if (::kill(pid, 0) < 0)
            return std::error_code(errno, std::system_category());
        else
            return std::error_code();
    }
}}}}
