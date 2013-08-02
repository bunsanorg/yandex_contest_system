#define BOOST_TEST_MODULE cgroup
#include <boost/test/unit_test.hpp>

#include "SingleControlGroupFixture.hpp"
#include "MultipleControlGroupFixture.hpp"

#include "yandex/contest/system/cgroup/SystemInfo.hpp"
#include "yandex/contest/system/cgroup/ProcessInfo.hpp"
#include "yandex/contest/system/cgroup/MountPoint.hpp"
#include "yandex/contest/system/cgroup/ControlGroup.hpp"
#include "yandex/contest/system/cgroup/CpuAccounting.hpp"

#include "bunsan/testing/exec_test.hpp"

#include <boost/scope_exit.hpp>

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
    const yac::SingleControlGroupPointer acg =
        yac::SingleControlGroup::attach(processHierarchy.hierarchy.id, cg->controlGroup());
    BOOST_CHECK_EQUAL(acg->tasks().size(), 0);
    BOOST_CHECK_EQUAL(acg->controlGroup(), cg->controlGroup());
    BOOST_CHECK_THROW(acg->attachChild("123"), yac::SingleControlGroupNotExistsError);
}

BOOST_AUTO_TEST_CASE(created)
{
    const yac::SingleControlGroupPointer thisCG2 =
        yac::SingleControlGroup::attach(processHierarchy.hierarchy.id, thisCG->controlGroup());
    BOOST_CHECK_THROW(thisCG2->createChild(cg->controlGroup().filename()), yac::SingleControlGroupExistsError);
}

BOOST_AUTO_TEST_CASE(parent)
{
    const yac::SingleControlGroupPointer pcg = cg->parent();
    BOOST_CHECK_EQUAL(pcg, thisCG);
}

BOOST_AUTO_TEST_CASE(attachChild)
{
    const yac::SingleControlGroupPointer acg = thisCG->attachChild(cg->controlGroup().filename());
    BOOST_CHECK_EQUAL(acg, cg);
}

BOOST_AUTO_TEST_CASE(attachTask)
{
    BOOST_CHECK(cg->tasks().empty());
    BOOST_SCOPE_EXIT_ALL(this)
    {
        thisCG->attachTask(pid);
        BOOST_CHECK(cg->tasks().empty());
    };
    cg->attachTask(pid);
    const yac::ControlGroup::Tasks tasks = cg->tasks();
    BOOST_CHECK(tasks.find(pid) != tasks.end());
}

BOOST_AUTO_TEST_CASE(tree)
{
    const boost::filesystem::path cg2Path = boost::filesystem::unique_path();
    const boost::filesystem::path cg3Path = boost::filesystem::unique_path();
    yac::SingleControlGroupPointer cg2 = thisCG->createChild(cg2Path);
    const yac::SingleControlGroupPointer cg3 = thisCG->createChild(cg3Path);
    BOOST_CHECK_NE(cg, cg2);
    BOOST_CHECK_NE(cg2, cg3);
    BOOST_CHECK_EQUAL(cg2->parent(), thisCG);
    BOOST_CHECK_EQUAL(thisCG->attachChild(cg2Path), cg2);
    BOOST_CHECK_EQUAL(thisCG->attachChild(cg3Path), cg3);
    const yac::SingleControlGroupPointer cg2_1 = cg2->createChild("1");
    {
        void *const cg2_ = cg2.get();
        cg2.reset();
        BOOST_CHECK_EQUAL(cg2_1->parent().get(), cg2_);
        BOOST_CHECK_EQUAL(thisCG->attachChild(cg2Path).get(), cg2_);
        cg2 = thisCG->attachChild(cg2Path);
    }
    BOOST_CHECK_EQUAL(thisCG->attachChild(""), thisCG);
    BOOST_CHECK_EQUAL(thisCG->attachChild("."), thisCG);
    BOOST_CHECK_EQUAL(thisCG->attachChild(cg2Path / ".."), thisCG);
    BOOST_CHECK_EQUAL(thisCG->attachChild(cg2Path / ".." / cg3Path), cg3);
}

BOOST_AUTO_TEST_SUITE_END() // SingleControlGroup

BOOST_FIXTURE_TEST_SUITE(MultipleControlGroup, MultipleControlGroupFixture)

BOOST_AUTO_TEST_CASE(forSelf)
{
    BOOST_TEST_MESSAGE(*thisCG);
}

BOOST_AUTO_TEST_CASE(parent)
{
    const yac::ControlGroup::Tasks tasks = cg->parent()->tasks();
    BOOST_CHECK(tasks.find(pid) != tasks.end());
}

BOOST_AUTO_TEST_CASE(fieldPath)
{
    BOOST_CHECK(boost::filesystem::is_regular_file(cg->fieldPath("freezer.state")));
    BOOST_CHECK_THROW(thisCG->fieldPath("/"), yac::ControlGroupInvalidFieldNameError);
    BOOST_CHECK_THROW(thisCG->fieldPath("unknown/path"), yac::ControlGroupInvalidFieldNameError);
    BOOST_CHECK_THROW(thisCG->fieldPath("unknown.path"), yac::ControlGroupFieldDoesNotExistError);
    BOOST_CHECK_THROW(thisCG->fieldPath(cgName.string()), yac::ControlGroupInvalidFieldFileError);
}

BOOST_AUTO_TEST_CASE(container)
{
    const std::size_t hierarchyId = yac::SystemInfo::instance()->bySubsystem("freezer").id;
    const yac::SingleControlGroupPointer cgroup = yac::SingleControlGroup::forSelf(hierarchyId);
    const yac::SingleControlGroupPointer cgroup2 = yac::SingleControlGroup::forSelf(hierarchyId);
    BOOST_CHECK_THROW(thisCG->add(cgroup), yac::MultipleControlGroupHierarchyConflictError);
    thisCG->remove(hierarchyId);
    BOOST_CHECK_THROW(thisCG->remove(hierarchyId), yac::MultipleControlGroupHierarchyNotFoundError);
    BOOST_CHECK(!thisCG->find(hierarchyId));
    thisCG->add(cgroup);
    BOOST_CHECK_EQUAL(thisCG->replace(cgroup2), cgroup);
}

BOOST_AUTO_TEST_SUITE_END() // MultipleControlGroup

BOOST_AUTO_TEST_SUITE_END() // cgroup
