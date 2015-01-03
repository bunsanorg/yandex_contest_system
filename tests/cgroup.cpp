#define BOOST_TEST_MODULE cgroup
#include <boost/test/unit_test.hpp>

#include "DummyProcess.hpp"
#include "SingleControlGroupFixture.hpp"
#include "MultipleControlGroupFixture.hpp"

#include <yandex/contest/system/cgroup/ControlGroup.hpp>
#include <yandex/contest/system/cgroup/CpuAccounting.hpp>
#include <yandex/contest/system/cgroup/MountPoint.hpp>
#include <yandex/contest/system/cgroup/ProcessInfo.hpp>
#include <yandex/contest/system/cgroup/SystemInfo.hpp>

#include <bunsan/testing/exec_test.hpp>

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
        thisCG->attachSelf();
        const yac::ControlGroup::Tasks tasks = thisCG->tasks();
        BOOST_CHECK(tasks.find(pid) != tasks.end());
        BOOST_CHECK(cg->tasks().empty());
    };
    cg->attachTask(pid); // do not replace with attachSelf()
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
    BOOST_CHECK_THROW(
        thisCG->fieldPath("/"),
        yac::ControlGroupInvalidFieldNameError
    );
    BOOST_CHECK_THROW(
        thisCG->fieldPath("unknown/path"),
        yac::ControlGroupInvalidFieldNameError
    );
    BOOST_CHECK_THROW(
        thisCG->fieldPath("unknown.path"),
        yac::ControlGroupFieldDoesNotExistError
    );
    BOOST_CHECK_THROW(
        thisCG->fieldPath(cgName.string()),
        yac::ControlGroupInvalidFieldFileError
    );
}

BOOST_AUTO_TEST_CASE(container)
{
    const std::size_t freezerId =
        yac::SystemInfo::instance()->bySubsystem("freezer").id;
    const yac::SingleControlGroupPointer cgroup =
        yac::SingleControlGroup::forSelf(freezerId);
    const yac::SingleControlGroupPointer cgroup2 =
        yac::SingleControlGroup::forSelf(freezerId);
    BOOST_CHECK_THROW(
        thisCG->add(cgroup),
        yac::MultipleControlGroupHierarchyConflictError
    );
    thisCG->remove(freezerId);
    BOOST_CHECK_THROW(
        thisCG->remove(freezerId),
        yac::MultipleControlGroupHierarchyNotFoundError
    );
    BOOST_CHECK(!thisCG->find(freezerId));
    thisCG->add(cgroup);
    BOOST_CHECK_EQUAL(thisCG->replace(cgroup2), cgroup);
}

BOOST_AUTO_TEST_CASE(fields)
{
    const std::size_t freezerId =
        yac::SystemInfo::instance()->bySubsystem("freezer").id;
    const yac::SingleControlGroupPointer freezerCG =
        yac::SingleControlGroup::forSelf(freezerId)->attachChild(cgName);
    cg->setCloneChildren();
    BOOST_CHECK(cg->cloneChildren());
    BOOST_CHECK(freezerCG->cloneChildren());
    cg->setCloneChildren(false);
    BOOST_CHECK(!cg->cloneChildren());
    BOOST_CHECK(!freezerCG->cloneChildren());
    freezerCG->setCloneChildren();
    BOOST_CHECK_THROW(
        cg->cloneChildren(),
        yac::MultipleControlGroupFieldValueConflictError
    );
    const std::vector<std::size_t> empty;
    BOOST_CHECK_THROW(
        yac::MultipleControlGroup::root(
            empty.begin(), empty.end()
        )->cloneChildren(),
        yac::EmptyMultipleControlGroupError
    );
}

BOOST_AUTO_TEST_CASE(tasks)
{
    std::unordered_map<std::size_t, yac::HierarchyInfo> hiers;
    for (const yac::HierarchyInfo &info: *yac::SystemInfo::instance())
    {
        if (hiers.find(info.id) == hiers.end())
        {
            hiers[info.id] = info;
            if (hiers.size() > 1)
                break;
        }
    }
    BOOST_REQUIRE_EQUAL(hiers.size(), 2);
    const yac::HierarchyInfo hier1 = hiers.begin()->second;
    hiers.erase(hiers.begin());
    const yac::HierarchyInfo hier2 = hiers.begin()->second;
    hiers.erase(hiers.begin());
    BOOST_REQUIRE(hiers.empty());

    const yac::SingleControlGroupPointer cg1 =
        yac::SingleControlGroup::forSelf(hier1.id)->attachChild(cgName);
    const yac::SingleControlGroupPointer cg2 =
        yac::SingleControlGroup::forSelf(hier2.id)->attachChild(cgName);

    ya::testing::DummyProcess p1, p2, p3;

    cg1->attachTask(p1.pid());
    {
        const yac::ControlGroup::Tasks &tasks = cg->tasks();
        BOOST_CHECK_EQUAL(tasks.size(), 1);
        BOOST_CHECK(tasks.find(p1.pid()) != tasks.end());

        const yac::ControlGroup::Tasks &t1 = cg1->tasks();
        BOOST_CHECK_EQUAL(t1.size(), 1);
        BOOST_CHECK(t1.find(p1.pid()) != t1.end());

        const yac::ControlGroup::Tasks &t2 = cg2->tasks();
        BOOST_CHECK_EQUAL(t2.empty(), 1);
    }
    cg2->attachTask(p2.pid());
    {
        const yac::ControlGroup::Tasks &tasks = cg->tasks();
        BOOST_CHECK_EQUAL(tasks.size(), 2);
        BOOST_CHECK(tasks.find(p1.pid()) != tasks.end());
        BOOST_CHECK(tasks.find(p2.pid()) != tasks.end());

        const yac::ControlGroup::Tasks &t1 = cg1->tasks();
        BOOST_CHECK_EQUAL(t1.size(), 1);
        BOOST_CHECK(t1.find(p1.pid()) != t1.end());

        const yac::ControlGroup::Tasks &t2 = cg2->tasks();
        BOOST_CHECK_EQUAL(t2.size(), 1);
        BOOST_CHECK(t2.find(p2.pid()) != t2.end());
    }
    cg->attachTask(p3.pid());
    {
        const yac::ControlGroup::Tasks &tasks = cg->tasks();
        BOOST_CHECK_EQUAL(tasks.size(), 3);
        BOOST_CHECK(tasks.find(p1.pid()) != tasks.end());
        BOOST_CHECK(tasks.find(p2.pid()) != tasks.end());
        BOOST_CHECK(tasks.find(p3.pid()) != tasks.end());

        const yac::ControlGroup::Tasks &t1 = cg1->tasks();
        BOOST_CHECK_EQUAL(t1.size(), 2);
        BOOST_CHECK(t1.find(p1.pid()) != t1.end());
        BOOST_CHECK(t1.find(p3.pid()) != t1.end());

        const yac::ControlGroup::Tasks &t2 = cg2->tasks();
        BOOST_CHECK_EQUAL(t2.size(), 2);
        BOOST_CHECK(t2.find(p2.pid()) != t2.end());
        BOOST_CHECK(t2.find(p3.pid()) != t2.end());
    }
}

BOOST_AUTO_TEST_SUITE_END() // MultipleControlGroup

BOOST_AUTO_TEST_SUITE_END() // cgroup
