#include "yandex/contest/system/cgroup/Freezer.hpp"

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
        setState(State::FROZEN);
    }

    void FreezerBase::unfreeze() const
    {
        setState(State::THAWED);
    }
}}}}
