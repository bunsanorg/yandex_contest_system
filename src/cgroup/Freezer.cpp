#include <yandex/contest/system/cgroup/Freezer.hpp>

#include <yandex/contest/SystemError.hpp>

#include <cerrno>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    const std::string FreezerBase::SUBSYSTEM_NAME = "freezer";

    FreezerBase::State FreezerBase::state() const
    {
        return readField<State>("state");
    }

    void FreezerBase::setState(const State state_) const
    {
        do
        {
            writeField("state", state_);
        }
        while (state() != state_);
    }

    void FreezerBase::freeze() const
    {
        /*
         * From cgroup/freezer-subsystem:
         *
         * It's important to note that freezing can be incomplete. In that case we return
         * EBUSY. This means that some tasks in the cgroup are busy doing something that
         * prevents us from completely freezing the cgroup at this time. After EBUSY,
         * the cgroup will remain partially frozen -- reflected by freezer.state reporting
         * "FREEZING" when read. The state will remain "FREEZING" until one of these
         * things happens:
         *
         * 1) Userspace cancels the freezing operation by writing "THAWED" to
         *     the freezer.state file
         * 2) Userspace retries the freezing operation by writing "FROZEN" to
         *     the freezer.state file (writing "FREEZING" is not legal
         *     and returns EINVAL)
         * 3) The tasks that blocked the cgroup from entering the "FROZEN"
         *     state disappear from the cgroup's set of tasks.
         */
        do
        {
            try
            {
                setState(State::FROZEN);
            }
            catch (SystemError &e)
            {
                std::error_code *ec = e.get<SystemError::errorCode>();
                if (!ec)
                    throw;
                if (ec->category() != std::system_category())
                    throw;
                if (ec->value() != EBUSY)
                    throw;
            }
        }
        // It is possible to use flag variable here
        // but explicit check to kernel state seems to
        // be more reliable.
        while (state() != State::FROZEN);
    }

    void FreezerBase::unfreeze() const
    {
        setState(State::THAWED);
    }
}}}}
