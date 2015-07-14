#define BOOST_TEST_MODULE DynamicLoader
#include <boost/test/unit_test.hpp>

#include <yandex/contest/system/unistd/DynamicLoader.hpp>

namespace unistd = yandex::contest::system::unistd;

BOOST_AUTO_TEST_SUITE(DynamicLoader)

int function(int a, int b) { return a + b; }

static int sfunction(int a, int b) { return a + b; }

BOOST_AUTO_TEST_CASE(dladdr) {
  BOOST_TEST_MESSAGE("function: " << unistd::dladdr(&function));
  BOOST_TEST_MESSAGE("sfunction: " << unistd::dladdr(&sfunction));
}

BOOST_AUTO_TEST_SUITE_END()  // DynamicLoader
