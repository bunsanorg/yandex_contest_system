#define BOOST_TEST_MODULE ExecuteUtility
#include <boost/test/unit_test.hpp>

#include <yandex/contest/system/execution/AsyncProcess.hpp>
#include <yandex/contest/system/execution/ErrCall.hpp>

#include <bunsan/test/exec_test.hpp>

#include <boost/optional/optional_io.hpp>

namespace ya = yandex::contest::system::execution;

BOOST_AUTO_TEST_SUITE(ErrCall)

BOOST_AUTO_TEST_CASE(true_)
{
    const ya::Result result = ya::getErrCallArgv("true");
    BOOST_CHECK_EQUAL(result.exitStatus, 0);
    BOOST_CHECK(!result.termSig);
    BOOST_CHECK(result.out.empty());
    BOOST_CHECK(result.err.empty());
}

BOOST_AUTO_TEST_CASE(false_)
{
    const ya::Result result = ya::getErrCallArgv("false");
    BOOST_CHECK(result.exitStatus);
    if (result.exitStatus)
        BOOST_CHECK(result.exitStatus.get() != 0);
    BOOST_CHECK(!result.termSig);
    BOOST_CHECK(result.out.empty());
    BOOST_CHECK(result.err.empty());
}

// TODO test for non-executable path
// TODO test for PATH usage

BOOST_AUTO_TEST_CASE(echo0)
{
    const ya::Result result = ya::getErrCallArgv("sh", "-ce", "echo 123");
    BOOST_CHECK_EQUAL(result.exitStatus, 0);
    BOOST_CHECK(!result.termSig);
    BOOST_CHECK(result.out.empty());
    BOOST_CHECK(result.err.empty());
}

BOOST_AUTO_TEST_CASE(echo1)
{
    const ya::Result result = ya::getErrCallArgv("sh", "-ce", "echo -n 123 >&2");
    BOOST_CHECK_EQUAL(result.exitStatus, 0);
    BOOST_CHECK(!result.termSig);
    BOOST_CHECK(result.out.empty());
    BOOST_CHECK_EQUAL(result.err, "123");
}

BOOST_AUTO_TEST_CASE(echo2)
{
    const ya::Result result = ya::getErrCallArgv("sh", "-ce", "echo 123 >&2");
    BOOST_CHECK_EQUAL(result.exitStatus, 0);
    BOOST_CHECK(!result.termSig);
    BOOST_CHECK(result.out.empty());
    BOOST_CHECK_EQUAL(result.err, "123\n");
}

BOOST_AUTO_TEST_CASE(echo3)
{
    const ya::Result result = ya::getErrCallArgv("sh", "-ce", "echo `seq 1 9` >&2");
    BOOST_CHECK_EQUAL(result.exitStatus, 0);
    BOOST_CHECK(!result.termSig);
    BOOST_CHECK(result.out.empty());
    BOOST_CHECK_EQUAL(result.err, "1 2 3 4 5 6 7 8 9\n");
}

BOOST_AUTO_TEST_SUITE_END() // ErrCall

BOOST_AUTO_TEST_SUITE(AsyncProcess)

BOOST_AUTO_TEST_CASE(echo0)
{
    ya::AsyncProcess::Options opts;
    opts.executable = "echo";
    opts.arguments = {"echo", "-n", "123"};
    ya::AsyncProcess process(opts);
    const ya::Result result = process.wait();
    BOOST_CHECK_EQUAL(result.exitStatus, 0);
    BOOST_CHECK(!result.termSig);
    BOOST_CHECK_EQUAL(result.out, "123");
    BOOST_CHECK(result.err.empty());
}

BOOST_AUTO_TEST_CASE(echo1)
{
    ya::AsyncProcess::Options opts;
    opts.executable = "cat";
    opts.in = "arbitrary string";
    opts.arguments = {"cat"};
    ya::AsyncProcess process(opts);
    const ya::Result result = process.wait();
    BOOST_CHECK_EQUAL(result.exitStatus, 0);
    BOOST_CHECK(!result.termSig);
    BOOST_CHECK_EQUAL(result.out, opts.in);
    BOOST_CHECK(result.err.empty());
}

BOOST_AUTO_TEST_CASE(echo2)
{
    ya::AsyncProcess::Options opts;
    opts.executable = "sh";
    opts.in = "echo 'stdout text with spaces'\necho 'stderr text with spaces' >&2";
    opts.arguments = {"sh", "-se"};
    ya::AsyncProcess process(opts);
    const ya::Result result = process.wait();
    BOOST_CHECK_EQUAL(result.exitStatus, 0);
    BOOST_CHECK(!result.termSig);
    BOOST_CHECK_EQUAL(result.out, "stdout text with spaces\n");
    BOOST_CHECK_EQUAL(result.err, "stderr text with spaces\n");
}

BOOST_AUTO_TEST_SUITE_END() // AsyncProcess
