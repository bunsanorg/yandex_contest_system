#pragma once

#include "yandex/contest/system/unistd/Error.hpp"
#include "yandex/contest/system/unistd/CharStarStar.hpp"

#include <string>
#include <vector>
#include <unordered_map>

#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    struct InvalidEnvironmentKeyError: virtual Error
    {
        typedef boost::error_info<struct keyTag, std::string> key;
    };

    class Exec: private boost::noncopyable
    {
    public:
        typedef std::vector<std::string> ProcessArguments;
        typedef std::unordered_map<std::string, std::string> ProcessEnvironment;

    public:
        explicit Exec(const boost::filesystem::path &executable,
                      const ProcessArguments &arguments=ProcessArguments(),
                      const ProcessEnvironment &environment=ProcessEnvironment());

        const char *executable() const;
        char **argv();
        char **envp();

        /// System call, see exec(3).
        int execv() const noexcept;

        /// \copydoc execv()
        int execvp() const noexcept;

        /// \copydoc execv()
        int execve() const noexcept;

        /// \copydoc execv()
        int execvpe() const noexcept;

    private:
        /*!
         * According to environ(3),
         * "The statement about argv[] and envp[] being constants is included
         * to make explicit to future writers of language bindings that these
         * objects are completely constant."
         * So, it is safe to use it in system calls with constant Exec object.
         * Nevertheless, it is not safe to export it to public interface
         * without constant modifier.
         *
         * \warning Do not modify returned data.
         */
        char **argv_() const;

        /// \copydoc argv_()
        char **envp_() const;

    private:
        boost::filesystem::path executable_;
        CharStarStar argvWrapper_, envpWrapper_;
    };
}}}}
