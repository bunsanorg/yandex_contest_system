#include "yandex/contest/system/execution/AsyncProcess.hpp"

#include "yandex/contest/system/unistd/Exec.hpp"

#include "yandex/contest/SystemError.hpp"

#include "yandex/contest/detail/LogHelper.hpp"

#include <iterator>

#include <boost/filesystem/fstream.hpp>
#include <boost/assert.hpp>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

namespace yandex{namespace contest{namespace system{namespace execution
{
    AsyncProcess::AsyncProcess(AsyncProcess &&process)
    {
        this->swap(process);
    }

    AsyncProcess &AsyncProcess::operator=(AsyncProcess &&process)
    {
        this->swap(process);
        return *this;
    }

    void AsyncProcess::swap(AsyncProcess &process) noexcept
    {
        using boost::swap;
        swap(in_, process.in_);
        swap(out_, process.out_);
        swap(err_, process.err_);
        swap(pid_, process.pid_);
        swap(result_, process.result_);
    }

    namespace
    {
        const char *reopenDescriptor(const int descriptor, const int flags, const char *const path)
        {
            const int fd = ::open(path, flags);
            if (fd < 0)
                return "open";
            if (::dup2(fd, descriptor) < 0)
                return "dup2";
            return 0;
        }
        const char *prepareChild(const char *const in,
                                 const char *const out,
                                 const char *const err)
        {
            const char *what = 0;
            if (!what)
                what = reopenDescriptor(STDIN_FILENO, O_RDONLY, in);
            if (!what)
                what = reopenDescriptor(STDOUT_FILENO, O_WRONLY, out);
            if (!what)
                what = reopenDescriptor(STDERR_FILENO, O_WRONLY, err);
            return what;
        }
    }

    AsyncProcess::AsyncProcess(const Options &options):
        in_(options.in)
    {
        // unistd::Exec::Exec() may throw, it should be placed before ::fork()
        const unistd::Exec exec(options.executable, options.arguments);
        pid_ = ::fork();
        if (pid_ < 0)
        {
            BOOST_THROW_EXCEPTION(SystemError("fork"));
        }
        else if (pid_ > 0)
        { // parent
            // does nothing for now
        }
        else
        { // child
            const char *what = prepareChild(in_.path().c_str(),
                                            out_.path().c_str(),
                                            err_.path().c_str());
            if (!what)
            {
                if (options.usePath)
                {
                    exec.execvp();
                    what = "execvp";
                }
                else
                {
                    exec.execv();
                    what = "execv";
                }
            }
            ::perror(what);
            ::_exit(1);
        }
    }

    AsyncProcess::operator bool() const noexcept
    {
        return pid_;
    }

    const Result &AsyncProcess::wait()
    {
        BOOST_ASSERT_MSG(*this, "Invalid AsyncProcess instance.");
        while (!result_)
        {
            STREAM_DEBUG << "Trying to wait for process " << pid_ << ".";
            int statLoc;
            const ::pid_t rpid = ::waitpid(pid_, &statLoc, 0);
            BOOST_ASSERT_MSG(rpid, "Timeout is impossible.");
            if (rpid != pid_)
            {
                BOOST_ASSERT(rpid < 0);
                if (errno != EINTR)
                {
                    // it is impossible to wait for child
                    BOOST_THROW_EXCEPTION(SystemError("waitpid"));
                }
            }
            else
            {
                // process has terminated
                result_ = Result(statLoc);
                collectOutput();
                STREAM_DEBUG << "Process result was collected for process " << pid_ << ".";
            }
        }
        BOOST_ASSERT(result_);
        return result_.get();
    }

    const boost::optional<Result> &AsyncProcess::poll()
    {
        BOOST_ASSERT_MSG(*this, "Invalid AsyncProcess instance.");
        if (!result_)
        {
            STREAM_DEBUG << "Trying to wait for process " << pid_ << ".";
            int statLoc;
            const ::pid_t rpid = ::waitpid(pid_, &statLoc, WNOHANG);
            if (rpid != 0)
            {
                if (rpid == pid_)
                {
                    result_ = Result(statLoc);
                    collectOutput();
                    STREAM_DEBUG << "Process result was collected for process " << pid_ << ".";
                }
                else
                {
                    BOOST_ASSERT(rpid < 0);
                    BOOST_THROW_EXCEPTION(SystemError("waitpid"));
                }
            }
        }
        return result_;
    }

    namespace
    {
        std::string readAll(const boost::filesystem::path &path)
        {
            boost::filesystem::ifstream fin(path);
            if (!fin)
                BOOST_THROW_EXCEPTION(SystemError("open"));
            const std::string contents{std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>()};
            fin.close();
            if (!fin)
                BOOST_THROW_EXCEPTION(SystemError("close"));
            return contents;
        }
    }

    void AsyncProcess::collectOutput()
    {
        BOOST_ASSERT(result_);
        STREAM_DEBUG << "Trying to collect data from temporary file at " << out_.path() << ".";
        result_.get().out = readAll(out_.path());
        out_.remove();
        STREAM_DEBUG << "Trying to collect data from temporary file at " << err_.path() << ".";
        result_.get().err = readAll(err_.path());
        err_.remove();
    }

    void AsyncProcess::stop()
    {
        BOOST_ASSERT_MSG(*this, "Invalid AsyncProcess instance.");
        if (!poll())
        {
            if (!::kill(pid_, SIGKILL) < 0)
                // FIXME check ESRCH
                BOOST_THROW_EXCEPTION(SystemError("kill"));
            wait();
        }
    }

    AsyncProcess::~AsyncProcess()
    {
        if (*this)
        {
            try
            {
                stop();
            }
            catch (std::exception &e)
            {
                STREAM_ERROR << "Unable to terminate process (pid = " << pid_ << ") due to error: \"" <<
                                e.what() << "\" (ignored).";
            }
        }
    }
}}}}
