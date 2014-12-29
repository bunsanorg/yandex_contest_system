#pragma once

extern "C"
{
    int lxc_getstate(const char *name, const char *lxcpath);
}
