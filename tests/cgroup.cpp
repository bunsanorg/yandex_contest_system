#define BOOST_TEST_MODULE cgroup
#include <boost/test/unit_test.hpp>

#include "yandex/contest/system/execution/tests/BoostExecTest.hpp"

#include "yandex/contest/system/cgroup/MountPoint.hpp"
#include "yandex/contest/system/cgroup/ControlGroup.hpp"

#include "yandex/contest/system/execution/AsyncProcess.hpp"

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
        cg.remove();
    }

    yac::ControlGroup cg;
};

BOOST_FIXTURE_TEST_SUITE(ControlGroup, ControlGroupFixture)

BOOST_AUTO_TEST_CASE(create)
{
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

BOOST_AUTO_TEST_SUITE_END() // ControlGroup

BOOST_AUTO_TEST_SUITE_END() // cgroup
