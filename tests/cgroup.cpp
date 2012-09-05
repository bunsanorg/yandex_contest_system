#define BOOST_TEST_MODULE cgroup
#include <boost/test/unit_test.hpp>

#include "yandex/contest/system/cgroup/MountPoint.hpp"

namespace ya = yandex::contest::system::cgroup;

BOOST_AUTO_TEST_SUITE(cgroup)

BOOST_AUTO_TEST_CASE(MountPoint)
{
    BOOST_TEST_MESSAGE(ya::getMountPoint());
}

BOOST_AUTO_TEST_SUITE_END() // cgroup
