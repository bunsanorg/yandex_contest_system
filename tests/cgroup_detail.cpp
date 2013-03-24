#define BOOST_TEST_MODULE cgroup_detail
#include <boost/test/unit_test.hpp>

#include "yandex/contest/system/cgroup/detail/StreamableWrapper.hpp"

namespace yacd = yandex::contest::system::cgroup::detail;

BOOST_AUTO_TEST_SUITE(cgroup_detail)

BOOST_AUTO_TEST_CASE(OStreamableWrapper)
{
    int x = 5;
    yacd::OStreamableWrapper wrapper(x);
    std::ostringstream sout;
    x = 7;
    sout << wrapper;
    BOOST_CHECK_EQUAL(sout.str(), "7");
}

BOOST_AUTO_TEST_CASE(IStreamableWrapper)
{
    int x = 0;
    yacd::IStreamableWrapper wrapper(x);
    std::istringstream sin("10");
    sin >> x;
    BOOST_CHECK_EQUAL(x, 10);
}

BOOST_AUTO_TEST_SUITE_END() // cgroup_detail
