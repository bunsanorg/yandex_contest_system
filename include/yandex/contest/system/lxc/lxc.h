#pragma once

/*!
 * \file
 *
 * \brief Wrapper file for lxc/lxc.h
 *
 * lxc/lxc.h can't be directly included in C++ code because it contains
 * C++ errors (but it can be included in C).
 */

#ifdef __cplusplus
extern "C" {
#endif

int yandex_contest_system_lxc_getstate(const char *const name);

#ifdef __cplusplus
} // extern "C"
#endif
