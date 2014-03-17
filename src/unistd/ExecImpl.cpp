#include <yandex/contest/system/unistd/Exec.hpp>

#include <yandex/contest/SystemError.hpp>

#include <cerrno>

#include <unistd.h>

namespace yandex{namespace contest{namespace system{namespace unistd
{
#define YANDEX_UNISTD_EXEC_IMPL(X, ...) \
    void Exec::X(std::error_code &ec) const noexcept \
    { \
        ::X(executable(), argv_(), ##__VA_ARGS__); \
        ec.assign(errno, std::system_category()); \
    } \
    void Exec::X() const \
    { \
        std::error_code ec; \
        X(ec); \
        BOOST_THROW_EXCEPTION( \
            SystemError(ec, __func__) << \
            info::executable(executable_)); \
    }

    YANDEX_UNISTD_EXEC_IMPL(execv)

    YANDEX_UNISTD_EXEC_IMPL(execvp)

    YANDEX_UNISTD_EXEC_IMPL(execve, envp_())

    YANDEX_UNISTD_EXEC_IMPL(execvpe, envp_())
}}}}
