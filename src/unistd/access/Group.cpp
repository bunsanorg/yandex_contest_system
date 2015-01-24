#include <yandex/contest/system/unistd/access/Group.hpp>

#include <grp.h>

namespace yandex{namespace contest{namespace system{
    namespace unistd{namespace access
{
    Group Group::load(const ::group &gr)
    {
        Group g;
        g.name = gr.gr_name;
        g.passwd = gr.gr_passwd;
        g.gid = gr.gr_gid;
        for (char **c = gr.gr_mem; *c; ++c)
            g.members.push_back(*c);
        return g;
    }
}}}}}
