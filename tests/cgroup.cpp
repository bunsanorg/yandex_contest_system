#define BOOST_TEST_MODULE cgroup
#include <boost/test/unit_test.hpp>

#include "yandex/contest/system/cgroup/SystemInfo.hpp"
#include "yandex/contest/system/cgroup/ProcessInfo.hpp"
#include "yandex/contest/system/cgroup/MountPoint.hpp"
#include "yandex/contest/system/cgroup/ControlGroup.hpp"
#include "yandex/contest/system/cgroup/CpuAccounting.hpp"

#include "yandex/contest/system/execution/AsyncProcess.hpp"

#include "yandex/contest/system/unistd/Operations.hpp"

#include "bunsan/testing/exec_test.hpp"

#include <thread>
#include <chrono>

#include <csignal>

#include <boost/filesystem/operations.hpp>

namespace ya = yandex::contest::system;
namespace yac = ya::cgroup;
namespace unistd = ya::unistd;

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

BOOST_AUTO_TEST_CASE(MountPoint)
{
    BOOST_TEST_MESSAGE(yac::getMountPoint());
}

struct ControlGroupFixture
{
    ControlGroupFixture():
        pid(unistd::getpid()),
        thisCG(yac::ControlGroup::getControlGroup(pid)),
        cg(thisCG.createChild(boost::filesystem::unique_path()))
    {
    }

    ~ControlGroupFixture()
    {
        thisCG.attachTask(pid);
        cg.close();
        thisCG.close();
    }

    const pid_t pid;
    yac::ControlGroup thisCG;
    yac::ControlGroup cg;
};

BOOST_FIXTURE_TEST_SUITE(ControlGroup, ControlGroupFixture)

BOOST_AUTO_TEST_CASE(uninitialized)
{
    yac::ControlGroup ucg;
    BOOST_CHECK_THROW(ucg.terminate(), yac::InvalidControlGroupError);
}

BOOST_AUTO_TEST_CASE(attached)
{
    yac::ControlGroup acg("/", yac::ControlGroup::Attach);
    BOOST_CHECK_GT(acg.tasks().size(), 0);
    acg.attach(cg.name());
    BOOST_CHECK_EQUAL(acg.tasks().size(), 0);
    acg.close();
}

BOOST_AUTO_TEST_CASE(parent)
{
    yac::ControlGroup pcg = cg.parent();
    BOOST_CHECK_GT(pcg.tasks().size(), 0);
}

BOOST_AUTO_TEST_CASE(attachChild)
{
    yac::ControlGroup acg = thisCG.attachChild(cg.name());
    BOOST_CHECK_EQUAL(acg.path(), cg.path());
}

BOOST_AUTO_TEST_CASE(attachTask)
{
    cg.attachTask(pid);
    const yac::ControlGroup::Tasks tasks = cg.tasks();
    BOOST_CHECK(tasks.find(pid) != tasks.end());
}

BOOST_AUTO_TEST_CASE(getControlGroup)
{
    const yac::ControlGroup::Tasks tasks = thisCG.tasks();
    BOOST_CHECK(tasks.find(pid) != tasks.end());
}

BOOST_AUTO_TEST_CASE(terminate)
{
    ya::execution::AsyncProcess::Options opts;
    opts.executable = "sleep";
    opts.arguments = {"sleep", "60"};
    ya::execution::AsyncProcess process(opts);
    cg.attachTask(process.pid());
    cg.terminate();
    const ya::execution::Result result = process.wait();
    BOOST_CHECK(!result.exitStatus);
    BOOST_CHECK(result.termSig);
    if (result.termSig)
        BOOST_CHECK_EQUAL(result.termSig.get(), SIGKILL);
    process.wait();
}

BOOST_AUTO_TEST_CASE(fork_bomb)
{
    ya::execution::AsyncProcess::Options opts;
    opts.executable = "sh";
    const char cmd[] = R"EOF(
        bomb()
        (
            while true
            do
                bomb
            done
        )
        bomb
    )EOF";
    opts.arguments = {"sh", "-c", cmd};
    // TODO better implementation will not move
    // itself into cgroup
    cg.attachTask(pid);
    ya::execution::AsyncProcess process(opts);
    cg.parent().attachTask(pid);
    // let fork bomb spawn
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    BOOST_TEST_MESSAGE("Tasks has started: " << cg.tasks().size());
    BOOST_CHECK_GT(cg.tasks().size(), 1);
    cg.remove();
}

BOOST_AUTO_TEST_SUITE_END() // ControlGroup

BOOST_FIXTURE_TEST_SUITE(Subsystems, ControlGroupFixture)

BOOST_AUTO_TEST_CASE(CpuAccounting)
{
    const yac::CpuAccounting cpuAcct(thisCG);
    BOOST_TEST_MESSAGE("user: " << cpuAcct.userUsage().count());
    BOOST_TEST_MESSAGE("system: " << cpuAcct.systemUsage().count());
    BOOST_TEST_MESSAGE("usage: " << cpuAcct.usage().count());
}

BOOST_AUTO_TEST_SUITE_END() // Subsystems

BOOST_AUTO_TEST_SUITE_END() // cgroup
