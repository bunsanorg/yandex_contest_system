#define BOOST_TEST_MODULE cgroup
#include <boost/test/unit_test.hpp>

#include "yandex/contest/system/cgroup/MountPoint.hpp"
#include "yandex/contest/system/cgroup/ControlGroup.hpp"

#include <boost/filesystem/operations.hpp>

namespace ya = yandex::contest::system::cgroup;

BOOST_AUTO_TEST_SUITE(cgroup)

BOOST_AUTO_TEST_CASE(MountPoint)
{
    BOOST_TEST_MESSAGE(ya::getMountPoint());
}

BOOST_AUTO_TEST_SUITE(ControlGroup)

BOOST_AUTO_TEST_CASE(create)
{
    ya::ControlGroup cg(boost::filesystem::unique_path());
}

BOOST_AUTO_TEST_SUITE_END() // ControlGroup

BOOST_AUTO_TEST_SUITE_END() // cgroup
