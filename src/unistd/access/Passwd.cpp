#include <yandex/contest/system/unistd/access/Passwd.hpp>

#include <pwd.h>

namespace yandex{namespace contest{namespace system{
    namespace unistd{namespace access
{
    Passwd Passwd::load(const ::passwd &pw)
    {
        Passwd p;
        p.name = pw.pw_name;
        p.passwd = pw.pw_passwd;
        p.id = Id(pw.pw_uid, pw.pw_gid);
        p.gecos = pw.pw_gecos;
        p.dir = pw.pw_dir;
        p.shell = pw.pw_shell;
        return p;
    }
}}}}}
