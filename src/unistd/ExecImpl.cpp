#include "yandex/contest/system/unistd/Exec.hpp"

#include <unistd.h>

// FIXME throw exception

namespace yandex{namespace contest{namespace system{namespace unistd
{
    int Exec::execv() const noexcept
    {
        return ::execv(executable(), argv_());
    }

    int Exec::execvp() const noexcept
    {
        return ::execvp(executable(), argv_());
    }

    int Exec::execve() const noexcept
    {
        return ::execve(executable(), argv_(), envp_());
    }

    int Exec::execvpe() const noexcept
    {
        return ::execvpe(executable(), argv_(), envp_());
    }
}}}}
