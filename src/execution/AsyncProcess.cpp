#include <yandex/contest/system/execution/AsyncProcess.hpp>

#include <yandex/contest/system/unistd/Exec.hpp>
#include <yandex/contest/system/unistd/Operations.hpp>

#include <yandex/contest/detail/LogHelper.hpp>
#include <yandex/contest/SystemError.hpp>

#include <bunsan/filesystem/fstream.hpp>

#include <boost/assert.hpp>

#include <iterator>

#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/wait.h>

namespace yandex{namespace contest{namespace system{namespace execution
{
    AsyncProcess::AsyncProcess(AsyncProcess &&process) noexcept
    {
        swap(process);
    }

    AsyncProcess &AsyncProcess::operator=(AsyncProcess &&process) noexcept
    {
        swap(process);
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
        void reopenDescriptor(const int descriptor, const int flags,
                              const boost::filesystem::path &path)
        {
            const unistd::Descriptor fd = unistd::open(path, flags);
            unistd::dup2(fd.get(), descriptor);
        }

        void prepareChild(const boost::filesystem::path &in,
                          const boost::filesystem::path &out,
                          const boost::filesystem::path &err)
        {
            reopenDescriptor(STDIN_FILENO, O_RDONLY, in);
            reopenDescriptor(STDOUT_FILENO, O_WRONLY, out);
            reopenDescriptor(STDERR_FILENO, O_WRONLY, err);
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
            try
            {
                prepareChild(in_.path(), out_.path(), err_.path());
                if (options.usePath)
                    exec.execvp();
                else
                    exec.execv();
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << std::endl;
                std::abort();
            }
            catch (...)
            {
                BOOST_ASSERT_MSG(false, "It should not happen, but should be checked.");
            }
        }
    }

    AsyncProcess::operator bool() const noexcept
    {
        return pid_;
    }

    AsyncProcess::Pid AsyncProcess::pid() const noexcept
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
            bunsan::filesystem::ifstream fin(path);
            BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin)
            {
                const std::string contents{
                    std::istreambuf_iterator<char>(fin),
                    std::istreambuf_iterator<char>()
                };
                fin.close();
                return contents;
            }
            BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
        }
    }

    void AsyncProcess::collectOutput()
    {
        BOOST_ASSERT(result_);
        STREAM_DEBUG << "Trying to collect data from temporary file at " << out_.path() << ".";
        result_->out = readAll(out_.path());
        out_.remove();
        STREAM_DEBUG << "Trying to collect data from temporary file at " << err_.path() << ".";
        result_->err = readAll(err_.path());
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
