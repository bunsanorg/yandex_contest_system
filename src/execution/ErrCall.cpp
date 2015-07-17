#include <yandex/contest/system/execution/ErrCall.hpp>

#include <yandex/contest/system/unistd/Pipe.hpp>
#include <yandex/contest/system/unistd/Exec.hpp>
#include <yandex/contest/system/unistd/Operations.hpp>

#include <yandex/contest/Log.hpp>
#include <yandex/contest/SystemError.hpp>

#include <bunsan/log/fallback.hpp>

#include <boost/assert.hpp>

#include <ext/stdio_filebuf.h>

#include <cerrno>
#include <cstdio>
#include <cstring>

#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/wait.h>

namespace yandex {
namespace contest {
namespace system {
namespace execution {

namespace {
void executeChildInit(unistd::Pipe &errPipe) noexcept {
  const unistd::Descriptor devNull = unistd::open("/dev/null", O_RDWR);
  // we do not allow child process to interfere with parent's stdin
  unistd::dup2(devNull.get(), STDIN_FILENO);
  // we do not want to get annoying useless messages on console
  unistd::dup2(devNull.get(), STDOUT_FILENO);
  unistd::dup2(errPipe.writeEnd(), STDERR_FILENO);
  errPipe.closeReadEnd();
  errPipe.closeWriteEnd();
}  // namespace

/// Function never returns.
void executeChildFunction(const unistd::Exec &exec, const bool usePath,
                          unistd::Pipe &errPipe) noexcept {
  try {
    executeChildInit(errPipe);
    if (usePath) {
      exec.execvp();
    } else {
      exec.execv();
    }
  } catch (std::exception &e) {
    BUNSAN_LOG_FATAL_INTO(std::cerr) << "Unable to start due to: " << e.what();
    std::abort();
  } catch (...) {
    BUNSAN_LOG_FATAL_INTO(std::cerr) << "Unable to start due to unknown error";
    std::abort();
  }
}

void executeParentFunction(const ::pid_t pid, unistd::Pipe &errPipe,
                           Result &result) {
  std::error_code ec;
  // errors are ignored
  errPipe.closeWriteEnd(ec);
  __gnu_cxx::stdio_filebuf<char> errBuf(errPipe.readEnd(), std::ios::in);
  std::istream errS(&errBuf);
  char buf[BUFSIZ];
  do {
    errS.read(buf, std::min((std::size_t)BUFSIZ,
                            result.err.capacity() - result.err.size()));
    // string is preallocated,
    // so this should not cause an exception
    result.err.append(buf, errS.gcount());
  } while (errS && result.err.size() < result.err.capacity());
  // we will ignore all errors connected with IO
  // but we need to close descriptor
  // to not cause any child process block
  errPipe.closeReadEnd(ec);
  // let's collect process exit info
  int statLoc;
  // wait function can be interrupted
  // so we will use the loop to try again
  for (;;) {
    const ::pid_t rpid = ::waitpid(pid, &statLoc, 0);
    BOOST_ASSERT_MSG(rpid, "Timeout is impossible.");
    if (rpid == pid) {
      // child has terminated
      result.assign(statLoc);
      break;
    } else {
      BOOST_ASSERT(rpid < 0);
      if (errno != EINTR) {
        // it is impossible to wait for child
        BOOST_THROW_EXCEPTION(SystemError("waitpid"));
      }
    }
  }
}
}  // namespace

constexpr std::string::size_type MAX_ERR_SIZE = 10000;

Result getErrCallImpl(const std::string &executable,
                      const ProcessArguments &arguments, const bool usePath) {
  unistd::Pipe errPipe;
  Result result;
  result.err.reserve(MAX_ERR_SIZE);
  // unistd::Exec::Exec() may throw, it should be placed before ::fork()
  const unistd::Exec exec(executable, arguments);
  // be careful with exceptions
  const ::pid_t pid = fork();
  if (pid < 0) {
    // it is not possible to start new process
    BOOST_THROW_EXCEPTION(SystemError("fork"));
  } else if (pid > 0) {
    executeParentFunction(pid, errPipe, result);
  } else {
    Log::disableLogging();
    executeChildFunction(exec, usePath, errPipe);
  }
  return result;
}

}  // namespace execution
}  // namespace system
}  // namespace contest
}  // namespace yandex
