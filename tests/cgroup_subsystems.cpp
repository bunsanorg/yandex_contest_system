#define BOOST_TEST_MODULE cgroup_subsystems
#include <boost/test/unit_test.hpp>

#include "SingleControlGroupFixture.hpp"

#include "yandex/contest/system/cgroup/ControlGroup.hpp"
#include "yandex/contest/system/cgroup/CpuAccounting.hpp"

#include "bunsan/testing/exec_test.hpp"

BOOST_AUTO_TEST_SUITE(cgroup_subsystems)

#if 0
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
#endif

BOOST_AUTO_TEST_SUITE_END() // ControlGroup

BOOST_FIXTURE_TEST_SUITE(Subsystems, SingleControlGroupFixture)

BOOST_AUTO_TEST_CASE(CpuAccounting)
{
    const yac::CpuAccounting cpuAcct(thisCG);
    BOOST_TEST_MESSAGE("user: " << cpuAcct.userUsage().count());
    BOOST_TEST_MESSAGE("system: " << cpuAcct.systemUsage().count());
    BOOST_TEST_MESSAGE("usage: " << cpuAcct.usage().count());
}

BOOST_AUTO_TEST_SUITE_END() // cgroup_subsystems
