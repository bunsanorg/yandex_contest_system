#include <yandex/contest/system/unistd/Fstab.hpp>
#include <yandex/contest/system/unistd/CharStarStar.hpp>

#include <yandex/contest/SystemError.hpp>

#include <memory>

#include <boost/assert.hpp>

#include <stdio.h>
#include <mntent.h>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    namespace
    {
        struct EndMntEnt
        {
            void operator()(FILE *file) const
            {
                BOOST_VERIFY(endmntent(file) == 1);
            }
        };
    }

    void Fstab::load(const boost::filesystem::path &path)
    {
        struct mntent mntbuf;
        // hope it is enough
        constexpr int buflen = 8 * 1024;
        char buf[buflen];
        const std::unique_ptr<FILE, EndMntEnt> file(setmntent(path.c_str(), "r"));
        if (!file)
            BOOST_THROW_EXCEPTION(SystemError("setmntent (fopen)"));
        entries.clear();
        struct mntent *mntent;
        while ((mntent = getmntent_r(file.get(), &mntbuf, buf, buflen)))
        {
            MountEntry entry;
            entry.fsname = mntent->mnt_fsname;
            entry.dir = mntent->mnt_dir;
            entry.type = mntent->mnt_type;
            entry.opts = mntent->mnt_opts;
            entry.freq = mntent->mnt_freq;
            entry.passno = mntent->mnt_passno;
            entries.push_back(entry);
        }
    }

    void Fstab::save(const boost::filesystem::path &path) const
    {
        struct mntent mntbuf;
        const std::unique_ptr<FILE, EndMntEnt> file(setmntent(path.c_str(), "w"));
        if (!file)
            BOOST_THROW_EXCEPTION(SystemError("setmntent (fopen)"));
        for (const MountEntry &ent: entries)
        {
            std::vector<char> fsname = CharStarStar::stringToVectorChar(ent.fsname);
            mntbuf.mnt_fsname = &fsname[0];
            std::vector<char> dir = CharStarStar::stringToVectorChar(ent.dir);
            mntbuf.mnt_fsname = &dir[0];
            std::vector<char> type = CharStarStar::stringToVectorChar(ent.type);
            mntbuf.mnt_fsname = &type[0];
            std::vector<char> opts = CharStarStar::stringToVectorChar(ent.opts);
            mntbuf.mnt_fsname = &opts[0];
            mntbuf.mnt_freq = ent.freq;
            mntbuf.mnt_passno = ent.passno;
            if (addmntent(file.get(), &mntbuf))
                BOOST_THROW_EXCEPTION(SystemError("addmntent"));
        }
    }
}}}}
