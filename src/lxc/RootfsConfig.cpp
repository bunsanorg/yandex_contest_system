#include "yandex/contest/system/lxc/RootfsConfig.hpp"

#include "yandex/contest/system/lxc/ConfigHelper.hpp"

namespace yandex{namespace contest{namespace system{namespace lxc
{
    void RootfsConfig::patch(const RootfsConfig &config)
    {
        BOOST_OPTIONAL_OVERRIDE_PATCH(fsname);
        BOOST_OPTIONAL_OVERRIDE_PATCH(mount);
    }

    std::ostream &operator<<(std::ostream &out, const RootfsConfig &config)
    {
        config_helper::optionalOutput(out, "lxc.rootfs", config.fsname);
        config_helper::optionalOutput(out, "lxc.rootfs.mount", config.mount);
        return out;
    }
}}}}
