#include "yandex/contest/system/lxc/MountConfig.hpp"

#include "yandex/contest/system/lxc/ConfigHelper.hpp"

namespace yandex{namespace contest{namespace system{namespace lxc
{
    void MountConfig::patch(const MountConfig &config)
    {
        BOOST_OPTIONAL_OVERRIDE_PATCH(fstab);
        BOOST_OPTIONAL_OVERRIDE_PATCH(entries);
    }

    std::ostream &operator<<(std::ostream &out, const MountConfig &config)
    {
        config_helper::optionalOutput(out, "lxc.mount", config.fstab);
        if (config.entries)
            for (const unistd::MountEntry &entry: config.entries.get())
                config_helper::output(out, "lxc.mount.entry", entry);
        return out;
    }
}}}}
