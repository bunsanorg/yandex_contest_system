#include "yandex/contest/system/unistd/MountEntry.hpp"

namespace yandex{namespace contest{namespace system{namespace unistd
{
    MountEntry MountEntry::bind(const boost::filesystem::path &from,
                                const boost::filesystem::path &to,
                                const std::string &opts)
    {
        MountEntry entry;
        entry.fsname = from.string();
        entry.dir = to.string();
        entry.type = "none";
        entry.opts = "bind";
        if (!opts.empty())
        {
            entry.opts.push_back(',');
            entry.opts.append(opts);
        }
        return entry;
    }

    MountEntry MountEntry::bindRO(const boost::filesystem::path &from,
                                  const boost::filesystem::path &to)
    {
        return bind(from, to, "ro");
    }

    MountEntry MountEntry::proc()
    {
        MountEntry entry;
        entry.fsname = "proc";
        entry.dir = "/proc";
        entry.type = "proc";
        entry.opts = "defaults";
        return entry;
    }
}}}}
