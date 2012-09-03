#include "yandex/contest/system/unistd/ProcessResult.hpp"

#include <sys/wait.h>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    ProcessResult::ProcessResult(const int statLoc)
    {
        assign(statLoc);
    }

    void ProcessResult::assign(const int statLoc)
    {
        if (WIFEXITED(statLoc))
            exitStatus = WEXITSTATUS(statLoc);
        if (WIFSIGNALED(statLoc))
            termSig = WTERMSIG(statLoc);
    }

    ProcessResult::operator bool() const
    {
        if (termSig)
            return false;
        if (!exitStatus)
            return false;
        return exitStatus.get() == 0;
    }
}}}}
