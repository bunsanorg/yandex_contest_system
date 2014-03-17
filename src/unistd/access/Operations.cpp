#include <yandex/contest/system/unistd/access/Id.hpp>

#include <yandex/contest/system/unistd/Operations.hpp>

namespace yandex{namespace contest{namespace system{
    namespace unistd{namespace access
{
    void getRealEffectiveSavedId(Id &real, Id &effective, Id &saved) noexcept
    {
        getresuid(real.uid, effective.uid, saved.uid);
        getresgid(real.gid, effective.gid, saved.gid);
    }

    Id getRealId() noexcept
    {
        return {getuid(), getgid()};
    }

    Id getEffectiveId() noexcept
    {
        return {geteuid(), getegid()};
    }

    Id getSavedId() noexcept
    {
        Id real, effective, saved;
        getRealEffectiveSavedId(real, effective, saved);
        return saved;
    }

    void dropId(const Id &id)
    {
        // order matters
        setgid(id.gid);
        setuid(id.uid);
    }

    void setRealEffectiveSavedId(
        const Id &real, const Id &effective, const Id &saved)
    {
        setresuid(real.uid, effective.uid, saved.uid);
        setresgid(real.gid, effective.gid, saved.gid);
    }

    void setId(const Id &id)
    {
        setuid(id.uid);
        setgid(id.gid);
    }

    void setEffectiveId(const Id &id)
    {
        seteuid(id.uid);
        setegid(id.gid);
    }
}}}}}
