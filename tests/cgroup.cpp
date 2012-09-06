#define BOOST_TEST_MODULE cgroup
#include <boost/test/unit_test.hpp>

#include "yandex/contest/system/execution/tests/BoostExecTest.hpp"

#include "yandex/contest/system/cgroup/MountPoint.hpp"
#include "yandex/contest/system/cgroup/ControlGroup.hpp"

#include "yandex/contest/system/execution/AsyncProcess.hpp"

#include <thread>
#include <chrono>

#include <csignal>

#include <boost/filesystem/operations.hpp>

namespace ya = yandex::contest::system;
namespace yac = ya::cgroup;

BOOST_AUTO_TEST_SUITE(cgroup)

BOOST_AUTO_TEST_CASE(MountPoint)
{
    BOOST_TEST_MESSAGE(yac::getMountPoint());
}

struct ControlGroupFixture
{
    ControlGroupFixture():
        cg(boost::filesystem::unique_path())
    {
    }

    ~ControlGroupFixture()
    {
        cg.close();
    }

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

BOOST_AUTO_TEST_CASE(terminate)
{
    ya::execution::AsyncProcess::Options opts;
    opts.executable = "sleep";
    opts.arguments = {"sleep", "60"};
    ya::execution::AsyncProcess process(opts);
    cg.attach(process.pid());
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
    cg.attach(getpid());
    ya::execution::AsyncProcess process(opts);
    cg.parent().attach(getpid());
    // let fork bomb spawn
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    BOOST_TEST_MESSAGE("Tasks has started: " << cg.tasks().size());
    BOOST_CHECK_GT(cg.tasks().size(), 1);
    cg.remove();
}

BOOST_AUTO_TEST_SUITE_END() // ControlGroup

BOOST_AUTO_TEST_SUITE_END() // cgroup
