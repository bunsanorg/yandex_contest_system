#include "yandex/contest/system/cgroup/MountPoint.hpp"

#include "yandex/contest/system/unistd/Fstab.hpp"

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    boost::filesystem::path getMountPoint()
    {
        unistd::Fstab fstab;
        fstab.load("/proc/mounts");
        for (const unistd::MountEntry &entry: fstab)
        {
            if (entry.type == "cgroup")
                return entry.dir;
        }
        BOOST_THROW_EXCEPTION(ControlGroupIsNotMountedError());
    }
}}}}
