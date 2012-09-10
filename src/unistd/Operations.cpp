#include "yandex/contest/system/unistd/Operations.hpp"

#include "yandex/contest/SystemError.hpp"

#include <csignal>

#include <boost/assert.hpp>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/syscall.h>

// internal defines

#define YANDEX_UNISTD_WRAP_X(X, INFO) if ((X) < 0) BOOST_THROW_EXCEPTION(SystemError(__func__) INFO)

#define YANDEX_UNISTD_WRAP(X, INFO) YANDEX_UNISTD_WRAP_X(X, << INFO)
#define YANDEX_UNISTD_WRAP_NO_INFO(X) YANDEX_UNISTD_WRAP_X(X, )

#define YANDEX_UNISTD_RETURN_X(X, INFO) \
    const auto ret = X; \
    if (ret < 0) \
        BOOST_THROW_EXCEPTION(SystemError(__func__) INFO); \
    else \
        return ret

#define YANDEX_UNISTD_RETURN(X, INFO) YANDEX_UNISTD_RETURN_X(X, << INFO)
#define YANDEX_UNISTD_RETURN_NO_INFO(X) YANDEX_UNISTD_RETURN_X(X, )

#define YANDEX_UNISTD_ASSIGN_X(X, OBJ, INFO) \
    const auto ret = X; \
    if (ret < 0) \
        BOOST_THROW_EXCEPTION(SystemError(__func__) INFO); \
    else \
        OBJ.assign(ret);

#define YANDEX_UNISTD_ASSIGN(X, OBJ, INFO) YANDEX_UNISTD_ASSIGN_X(X, OBJ, << INFO)
#define YANDEX_UNISTD_ASSIGN_NO_INFO(X, OBJ) YANDEX_UNISTD_ASSIGN_X(X, OBJ, )

namespace yandex{namespace contest{namespace system{namespace unistd
{
    void chmod(const boost::filesystem::path &path, const mode_t mode)
    {
        YANDEX_UNISTD_WRAP(::chmod(path.c_str(), mode),
                           info::path(path) << info::mode(mode));
    }

    void chown(const boost::filesystem::path &path, const access::Id &id)
    {
        YANDEX_UNISTD_WRAP(::chown(path.c_str(), id.uid, id.gid),
                           info::path(path) << info::accessId(id));
    }

    void lchown(const boost::filesystem::path &path, const access::Id &id)
    {
        YANDEX_UNISTD_WRAP(::lchown(path.c_str(), id.uid, id.gid),
                           info::path(path) << info::accessId(id));
    }

    void mkdir(const boost::filesystem::path &path, const mode_t mode)
    {
        YANDEX_UNISTD_WRAP(::mkdir(path.c_str(), mode),
                           info::path(path) << info::mode(mode));
    }

    void rmdir(const boost::filesystem::path &path)
    {
        YANDEX_UNISTD_WRAP(::rmdir(path.c_str()), info::path(path));
    }

    void mknod(const boost::filesystem::path &path, const mode_t mode, const dev_t dev)
    {
        YANDEX_UNISTD_WRAP(::mknod(path.c_str(), mode, dev),
                           info::path(path) << info::mode(mode) <<
                           info::devMajor(major(dev)) << info::devMinor(minor(dev)));
    }

    dev_t makedev(const int major, const int minor)
    {
        return ::makedev(major, minor);
    }

    void symlink(const boost::filesystem::path &value, const boost::filesystem::path &path)
    {
        YANDEX_UNISTD_WRAP(::symlink(value.c_str(), path.c_str()),
                           info::path(path) << info::symLinkValue(value));
    }

    void mkfifo(const boost::filesystem::path &path, const mode_t mode)
    {
        YANDEX_UNISTD_WRAP(::mkfifo(path.c_str(), mode),
                           info::path(path) << info::mode(mode));
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
        YANDEX_UNISTD_WRAP(::stat(path.c_str(), &buf), info::path(path));
        return buf;
    }

    FileStatus fstat(int fd)
    {
        StatusType buf;
        YANDEX_UNISTD_WRAP(::fstat(fd, &buf), info::fd(fd));
        return buf;
    }

    FileStatus lstat(const boost::filesystem::path &path)
    {
        StatusType buf;
        YANDEX_UNISTD_WRAP(::lstat(path.c_str(), &buf), info::path(path));
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
        YANDEX_UNISTD_WRAP(::setuid(uid), info::uid(uid));
    }

    void setgid(const gid_t gid)
    {
        YANDEX_UNISTD_WRAP(::setgid(gid), info::gid(gid));
    }

    void setreuid(const uid_t ruid, const uid_t euid)
    {
        YANDEX_UNISTD_WRAP(::setreuid(ruid, euid),
                           info::ruid(ruid) << info::euid(euid));
    }

    void setregid(const gid_t rgid, const gid_t egid)
    {
        YANDEX_UNISTD_WRAP(::setregid(rgid, egid),
                           info::rgid(rgid) << info::egid(egid));
    }

    void setresuid(const uid_t ruid, const uid_t euid, const uid_t suid)
    {
        YANDEX_UNISTD_WRAP(::setresuid(ruid, euid, suid),
                           info::ruid(ruid) << info::euid(euid) << info::suid(suid));
    }

    void setresgid(const gid_t rgid, const gid_t egid, const gid_t sgid)
    {
        YANDEX_UNISTD_WRAP(::setresgid(rgid, egid, sgid),
                           info::rgid(rgid) << info::egid(egid) << info::sgid(sgid));
    }

    pid_t fork()
    {
        YANDEX_UNISTD_RETURN_NO_INFO(::fork());
    }

    Descriptor open(const boost::filesystem::path &path, const int oflag, const mode_t mode)
    {
        Descriptor retfd;
        YANDEX_UNISTD_ASSIGN(::open(path.c_str(), oflag, mode), retfd,
                             info::path(path) << info::openFlags(oflag) << info::mode(mode));
        return retfd;
    }

    void close(const int fd)
    {
        YANDEX_UNISTD_WRAP(::close(fd), info::fd(fd));
    }

    Descriptor dup(const int fd)
    {
        Descriptor retfd;
        YANDEX_UNISTD_ASSIGN(::dup(fd), retfd, info::fd(fd));
        return retfd;
    }

    void dup2(const int oldfd, const int newfd)
    {
        int retfd;
        YANDEX_UNISTD_WRAP(retfd = ::dup2(oldfd, newfd),
                           info::oldfd(oldfd) << info::newfd(newfd));
        BOOST_ASSERT(newfd == retfd);
    }

    unsigned getdtablesize()
    {
        YANDEX_UNISTD_RETURN_NO_INFO(::getdtablesize());
    }

    void getrlimit(const int resource, struct rlimit &rlp)
    {
        YANDEX_UNISTD_WRAP(::getrlimit(resource, &rlp), info::resource(resource));
    }

    void setrlimit(const int resource, const struct rlimit &rlp)
    {
        YANDEX_UNISTD_WRAP(::setrlimit(resource, &rlp), info::resource(resource));
    }

    void getitimer(const int which, ::itimerval &curr_value)
    {
        YANDEX_UNISTD_WRAP_NO_INFO(::getitimer(which, &curr_value));
    }

    void setitimer(const int which, const ::itimerval &new_value)
    {
        YANDEX_UNISTD_WRAP_NO_INFO(::setitimer(which, &new_value, nullptr));
    }

    void setitimer(const int which, const ::itimerval &new_value, ::itimerval &old_value)
    {
        YANDEX_UNISTD_WRAP_NO_INFO(::setitimer(which, &new_value, &old_value));
    }

    void kill(const pid_t pid, const int sig)
    {
        YANDEX_UNISTD_WRAP(::kill(pid, sig), info::pid(pid) << info::signal(sig));
    }

    std::error_code kill0(const pid_t pid) noexcept
    {
        if (::kill(pid, 0) < 0)
            return std::error_code(errno, std::system_category());
        else
            return std::error_code();
    }

    pid_t getpid() noexcept
    {
        return ::getpid();
    }

    pid_t gettid() noexcept
    {
        return ::syscall(SYS_gettid);
    }

    long sysconf(const int name)
    {
        YANDEX_UNISTD_RETURN(::sysconf(name), info::sysconfName(name));
    }
}}}}
