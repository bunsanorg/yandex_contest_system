#pragma once

#include <stdlib.h>

static const int hook = setenv("BOOST_TEST_CATCH_SYSTEM_ERRORS", "no", true);
