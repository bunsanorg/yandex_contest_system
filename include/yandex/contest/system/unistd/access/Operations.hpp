#pragma once

#include <yandex/contest/system/unistd/access/Group.hpp>
#include <yandex/contest/system/unistd/access/Id.hpp>
#include <yandex/contest/system/unistd/access/Passwd.hpp>

#include <vector>

namespace yandex{namespace contest{namespace system{
    namespace unistd{namespace access
{
    /// getresuid(2), getresgid(2)
    void getRealEffectiveSavedId(Id &real, Id &effective, Id &saved) noexcept;

    /// getuid(3), getgid(3)
    Id getRealId() noexcept;

    /// geteuid(3), getegid(3)
    Id getEffectiveId() noexcept;

    /// \see getRealEffectiveSavedId()
    Id getSavedId() noexcept;

    /*!
     * \brief This function should successfully drop
     * privileges from root to specified id.
     */
    void dropId(const Id &id);

    // FIXME these functions are broken
    /// setresuid(2), setresgid(2)
    //void setRealEffectiveSavedId(
    //    const Id &real, const Id &effective, const Id &saved);

    /// setuid(3), setgid(3)
    //void setId(const Id &id);

    /// seteuid(3), setegid(3)
    //void setEffectiveId(const Id &id);

    std::vector<Passwd> getPasswd();
    Passwd getPasswdByName(const std::string &name);
    Passwd getPasswdByUid(const uid_t uid);

    std::vector<Group> getGroup();
    Group getGroupByName(const std::string &name);
    Group getGroupByGid(const gid_t gid);
}}}}}
