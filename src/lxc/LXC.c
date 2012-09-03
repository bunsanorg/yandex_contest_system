#include "yandex/contest/system/lxc/LXC.h"

#include <lxc/lxc.h>

int yandex_contest_system_lxc_getstate(const char *const name)
{
    return lxc_getstate(name);
}
