#pragma once

#include <yandex/contest/system/unistd/access/Id.hpp>
#include <yandex/contest/system/unistd/Descriptor.hpp>
#include <yandex/contest/system/unistd/Error.hpp>
#include <yandex/contest/system/unistd/FileStatus.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

#include <chrono>
#include <system_error>

#include <csignal>

#include <sys/types.h>

struct rlimit;
struct itimerval;
struct epoll_event;

namespace yandex {
namespace contest {
namespace system {
namespace unistd {

namespace info {
using path = Error::path;
using mode = boost::error_info<struct modeTag, mode_t>;
using accessId = boost::error_info<struct accessIdTag, access::Id>;
using devMajor = boost::error_info<struct devMajorTag, int>;
using devMinor = boost::error_info<struct devMinorTag, int>;
using symLinkValue =
    boost::error_info<struct symLinkValueTag, boost::filesystem::path>;
using uid = boost::error_info<struct uidTag, uid_t>;
using ruid = boost::error_info<struct ruidTag, uid_t>;
using euid = boost::error_info<struct euidTag, uid_t>;
using suid = boost::error_info<struct suidTag, uid_t>;
using gid = boost::error_info<struct gidTag, gid_t>;
using rgid = boost::error_info<struct rgidTag, gid_t>;
using egid = boost::error_info<struct egidTag, gid_t>;
using sgid = boost::error_info<struct sgidTag, gid_t>;
using openFlags = boost::error_info<struct openFlagsTag, int>;
using fd = boost::error_info<struct fdTag, int>;
using oldFd = boost::error_info<struct oldFdTag, int>;
using newFd = boost::error_info<struct newFdTag, int>;
using inFd = boost::error_info<struct inFdTag, int>;
using outFd = boost::error_info<struct outFdTag, int>;
using resource = boost::error_info<struct resourceTag, int>;
using size = boost::error_info<struct sizeTag, std::size_t>;
using offset = boost::error_info<struct offsetTag, off_t>;
// TODO rlimit (how to do it without <sys/resource.h> include?
// TODO interval timer (the same...)
using pid = boost::error_info<struct pidTag, pid_t>;
using signal = boost::error_info<struct signalTag, int>;
using sysconfName = boost::error_info<struct sysconfNameTag, int>;
using epfd = boost::error_info<struct epfdTag, int>;
using op = boost::error_info<struct opTag, int>;
using maxevents = boost::error_info<struct maxeventsTag, unsigned>;
}  // namespace info

/// chmod(3)
void chmod(const boost::filesystem::path &path, mode_t mode);

/// chown(3)
void chown(const boost::filesystem::path &path, const access::Id &id);

/// lchown(3)
void lchown(const boost::filesystem::path &path, const access::Id &id);

/// mkdir(3)
void mkdir(const boost::filesystem::path &path, mode_t mode);

/*!
 * \brief Like mkdir(3), except it does not treat EEXIST as error.
 *
 * \see boost::filesystem::create_directory()
 *
 * \return false on EEXIST
 */
bool create_directory(const boost::filesystem::path &path, mode_t mode);

/// rmdir(3)
void rmdir(const boost::filesystem::path &path);

/// mknod(3)
void mknod(const boost::filesystem::path &path, mode_t mode, dev_t dev);

/// makedev(3)
dev_t makedev(int major, int minor);

/// symlink(3)
void symlink(const boost::filesystem::path &value,
             const boost::filesystem::path &path);

/// mkfifo(3)
void mkfifo(const boost::filesystem::path &path, mode_t mode);

/// stat(3)
FileStatus stat(const boost::filesystem::path &path);

/// same as stat() except it does not treat file-not-found as error
boost::optional<FileStatus> statOptional(const boost::filesystem::path &path);

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

/// seteuid(3)
void seteuid(const uid_t uid);

/// setegid(3)
void setegid(const gid_t gid);

/// setreuid(3)
void setreuid(uid_t ruid, uid_t euid);

/// setregid(3)
void setregid(gid_t rgid, gid_t egid);

/// setresuid(2)
void setresuid(uid_t ruid, uid_t euid, uid_t suid);

/// setresgid(2)
void setresgid(gid_t rgid, gid_t egid, gid_t sgid);

/// fork(3)
pid_t fork();

/// open(3)
Descriptor open(const boost::filesystem::path &path, int oflag,
                mode_t mode = 0666);

/// close(3)
void close(int fd);

/// dup(3)
Descriptor dup(int fd);

/*!
 * \brief dup2(3)
 *
 * \code{.cpp}
 * dup2(1, 2); // redirect stderr to stdout
 * \endcode
 */
void dup2(int oldFd, int newFd);

/// sendfile(2)
std::size_t sendfile(int outFd, int inFd, off_t &offset, std::size_t count);

/// sendfile(outFd, inFd, offset, /* unspecified buffer size */)
std::size_t sendfile(int outFd, int inFd, off_t &offset);

/// sendfile(outFd, inFd, nullptr, count)
std::size_t sendfile(int outFd, int inFd, std::size_t count);

/// sendfile(outFd, inFd, /* unspecified buffer size */)
std::size_t sendfile(int outFd, int inFd);

/// getdtablesize(3)
unsigned getdtablesize();

/// getrlimit(3)
void getrlimit(int resource, struct rlimit &rlp);

/// setrlimit(3)
void setrlimit(int resource, const struct rlimit &rlp);

/// getitimer(3)
void getitimer(int which, ::itimerval &curr_value);

/// setitimer(3)
void setitimer(int which, const ::itimerval &new_value);

/// setitimer(3)
void setitimer(int which, const ::itimerval &new_value, ::itimerval &old_value);

/// kill(3)
void kill(pid_t pid, int sig);

/*!
 * \brief kill(pid, 0)
 *
 * \return error if happened.
 */
std::error_code kill0(pid_t pid) noexcept;

/// getpid(3)
pid_t getpid() noexcept;

/// gettid(2)
pid_t gettid() noexcept;

/// sysconf(3)
long sysconf(int name);

/// epoll_create1(2)
Descriptor epoll_create1(int flags = 0);

/// epoll_ctl(2)
void epoll_ctl(int epfd, int op, int fd, ::epoll_event &event);

/// epoll_ctl(epfd, EPOLL_CTL_ADD, fd, event)
void epoll_ctl_add(int epfd, int fd, ::epoll_event &event);

/// epoll_ctl(epfd, EPOLL_CTL_MOD, fd, event)
void epoll_ctl_mod(int epfd, int fd, ::epoll_event &event);

/// epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr)
void epoll_ctl_del(int epfd, int fd);

/// epoll_wait(2)(epfd, events, maxevents, -1)
unsigned epoll_wait(int epfd, ::epoll_event *events, unsigned maxevents);

template <unsigned MaxEvents>
unsigned epoll_wait(const int epfd, ::epoll_event(&events)[MaxEvents]) {
  return epoll_wait(epfd, events, MaxEvents);
}

/// epoll_wait(2)
unsigned epoll_wait(int epfd, ::epoll_event *events, unsigned maxevents,
                    const std::chrono::milliseconds &timeout);

template <unsigned MaxEvents>
unsigned epoll_wait(int epfd, ::epoll_event(&events)[MaxEvents],
                    const std::chrono::milliseconds &timeout) {
  return epoll_wait(epfd, events, MaxEvents, timeout);
}

/// epoll_pwait(2)(epfd, events, maxevents, -1, sigmask)
unsigned epoll_pwait(int epfd, ::epoll_event *events, unsigned maxevents,
                     const sigset_t &sigmask);

template <unsigned MaxEvents>
unsigned epoll_pwait(const int epfd, ::epoll_event(&events)[MaxEvents],
                     const sigset_t &sigmask) {
  return epoll_pwait(epfd, events, MaxEvents, sigmask);
}

/// epoll_pwait(2)
unsigned epoll_pwait(int epfd, ::epoll_event *events, unsigned maxevents,
                     const std::chrono::milliseconds &timeout,
                     const sigset_t &sigmask);

template <unsigned MaxEvents>
unsigned epoll_pwait(const int epfd, ::epoll_event(&events)[MaxEvents],
                     const std::chrono::milliseconds &timeout,
                     const sigset_t &sigmask) {
  return epoll_pwait(epfd, events, MaxEvents, timeout, sigmask);
}

}  // namespace unistd
}  // namespace system
}  // namespace contest
}  // namespace yandex
