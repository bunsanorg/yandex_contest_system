#define BOOST_TEST_MODULE cgroup
#include <boost/test/unit_test.hpp>

#include "SingleControlGroupFixture.hpp"

#include "yandex/contest/system/cgroup/SystemInfo.hpp"
#include "yandex/contest/system/cgroup/ProcessInfo.hpp"
#include "yandex/contest/system/cgroup/MountPoint.hpp"
#include "yandex/contest/system/cgroup/ControlGroup.hpp"
#include "yandex/contest/system/cgroup/CpuAccounting.hpp"

#include "bunsan/testing/exec_test.hpp"

#include <chrono>
#include <sstream>
#include <thread>

#include <csignal>

BOOST_AUTO_TEST_SUITE(cgroup)

BOOST_AUTO_TEST_CASE(SystemInfo)
{
    const yac::SystemInfoPointer ptr = yac::SystemInfo::instance();
    BOOST_TEST_MESSAGE(*ptr);
    const yac::SystemInfoPointer ptr2 = yac::SystemInfo::instance();
    BOOST_CHECK_EQUAL(ptr2, ptr);
    const yac::SystemInfoPointer ptr3 = yac::SystemInfo::instance(true);
    BOOST_CHECK_NE(ptr3, ptr);
}

BOOST_AUTO_TEST_CASE(ProcessInfo)
{
    const yac::ProcessInfo info = yac::ProcessInfo::forSelf();
    BOOST_TEST_MESSAGE(info);
}

BOOST_FIXTURE_TEST_SUITE(SingleControlGroup, SingleControlGroupFixture)

BOOST_AUTO_TEST_CASE(attached)
{
    yac::SingleControlGroupPointer acg = yac::SingleControlGroup::root(processHierarchy.hierarchy.id);
    BOOST_CHECK_GT(acg->tasks().size(), 0);
    acg = acg->attachChild(cg->controlGroup());
    BOOST_CHECK_EQUAL(acg->tasks().size(), 0);
    acg->close();
}

BOOST_AUTO_TEST_CASE(parent)
{
    const yac::SingleControlGroupPointer pcg = cg->parent();
    BOOST_CHECK_GT(pcg->tasks().size(), 0);
    BOOST_CHECK_EQUAL(pcg->controlGroup(), cg->controlGroup());
}

BOOST_AUTO_TEST_CASE(attachChild)
{
    const yac::SingleControlGroupPointer acg = thisCG->attachChild(cg->controlGroup());
    BOOST_CHECK_EQUAL(acg->controlGroup(), cg->controlGroup());
}

BOOST_AUTO_TEST_CASE(attachTask)
{
    cg->attachTask(pid);
    const yac::ControlGroup::Tasks tasks = cg->tasks();
    BOOST_CHECK(tasks.find(pid) != tasks.end());
}

BOOST_AUTO_TEST_CASE(getControlGroup)
{
    const yac::ControlGroup::Tasks tasks = thisCG->tasks();
    BOOST_CHECK(tasks.find(pid) != tasks.end());
}

BOOST_AUTO_TEST_SUITE_END() // SingleControlGroup

BOOST_AUTO_TEST_SUITE_END() // cgroup
