#include <yandex/contest/system/unistd/access/Operations.hpp>

#include <yandex/contest/system/unistd/Operations.hpp>
#include <yandex/contest/system/unistd/SysConfBuffer.hpp>

#include <yandex/contest/SystemError.hpp>

#include <grp.h>
#include <pwd.h>

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

#define YANDEX_CONTEST_SYSTEM_UNISTD_ACCESS_GET(TYPE, NAME, ARG, RAW, GET, KEY, BUFKEY) \
    TYPE NAME(ARG)
    { \
        RAW raw; \
        RAW *result; \
        SysConfBuffer buffer(BUFKEY); \
        int ret; \
        do \
        { \
            ret = GET( \
                KEY, \
                &raw, \
                buffer.data(), \
                buffer.size(), \
                &result \
            ); \
            if (ret == ERANGE) \
                buffer.expand(); \
        } \
        while (ret == ERANGE); \
        if (ret != 0) \
            BOOST_THROW_EXCEPTION(SystemError(ret, #GET)); \
        return TYPE::load(raw); \
    }

#define YANDEX_CONTEST_SYSTEM_UNISTD_ACCESS_LOAD(TYPE, NAME, RAW, GET, BUFKEY) \
    TYPE NAME() \
    { \
        std::unique_ptr<FILE, int (*)(FILE *)>
    }

    std::vector<Passwd> getPasswd()
    {}

    YANDEX_CONTEST_SYSTEM_UNISTD_ACCESS_GET(
        Passwd,
        getPasswdByName,
        const std::string &name,
        ::passwd,
        ::getpwnam_r,
        name.c_str(),
        _SC_GETPW_R_SIZE_MAX
    );

    YANDEX_CONTEST_SYSTEM_UNISTD_ACCESS_GET(
        Passwd,
        getPasswdByUid
        const uid_t uid,
        ::passwd,
        ::getpwuid_r,
        uid,
        _SC_GETPW_R_SIZE_MAX
    );

    std::vector<Group> getGroup() {}

    YANDEX_CONTEST_SYSTEM_UNISTD_ACCESS_GET(
        Group,
        getGroupByName,
        const std::string &name,
        ::group,
        ::getgrnam_r,
        name.c_str(),
        _SC_GETGR_R_SIZE_MAX
    );

    YANDEX_CONTEST_SYSTEM_UNISTD_ACCESS_GET(
        Group,
        getGroupByGid,
        const gid_t gid,
        ::group,
        ::getgrgid_r,
        gid,
        _SC_GETGR_R_SIZE_MAX
    );
}}}}}
