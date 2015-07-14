#define BOOST_TEST_MODULE cgroup_subsystems
#include <boost/test/unit_test.hpp>

#include "DummyProcess.hpp"
#include "MultipleControlGroupFixture.hpp"

#include <yandex/contest/system/cgroup/ControlGroup.hpp>
#include <yandex/contest/system/cgroup/CpuAccounting.hpp>
#include <yandex/contest/system/cgroup/Termination.hpp>

#include <bunsan/test/exec_test.hpp>

#include <thread>

BOOST_AUTO_TEST_SUITE(cgroup_subsystems)

BOOST_FIXTURE_TEST_SUITE(Termination, MultipleControlGroupFixture)

template <typename Process>
void waitCheck(Process &process) {
  const ya::execution::Result result = process.wait();
  BOOST_CHECK(!result.exitStatus);
  BOOST_CHECK(result.termSig);
  if (result.termSig) BOOST_CHECK_EQUAL(result.termSig.get(), SIGKILL);
}

BOOST_AUTO_TEST_CASE(terminate) {
  ya::execution::AsyncProcess::Options opts;
  opts.executable = "sleep";
  opts.arguments = {"sleep", "60"};
  ya::execution::AsyncProcess process(opts);
  cg->attachTask(process.pid());
  BOOST_CHECK_EQUAL(yac::terminate(cg), 1);
  waitCheck(process);
  BOOST_CHECK_EQUAL(cg->tasks().size(), 0);
}

BOOST_AUTO_TEST_CASE(fork_bomb) {
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
  cg->attachTask(pid);
  ya::execution::AsyncProcess process(opts);
  cg->parent()->attachTask(pid);

  // let fork bomb spawn
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  BOOST_TEST_MESSAGE("Tasks has started: " << cg->tasks().size());
  BOOST_CHECK_GT(yac::terminate(cg), 1);
  waitCheck(process);
  BOOST_CHECK_EQUAL(cg->tasks().size(), 0);
}

BOOST_AUTO_TEST_SUITE(TerminationGuard)

BOOST_AUTO_TEST_CASE(default_constructor) {
  yac::TerminationGuard tg;  // does nothing
  BOOST_CHECK(!tg);
}

BOOST_AUTO_TEST_CASE(destructor) {
  ya::test::DummyProcess process;
  cg->attachTask(process.pid());
  {
    yac::TerminationGuard tg(cg);
    BOOST_CHECK(tg);
  }
  waitCheck(process);
  BOOST_CHECK_EQUAL(cg->tasks().size(), 0);
}

BOOST_AUTO_TEST_CASE(detach) {
  ya::test::DummyProcess process;
  cg->attachTask(process.pid());
  {
    yac::TerminationGuard tg(cg);
    BOOST_CHECK(tg);
    tg.detach();
    BOOST_CHECK(!tg);
  }
  BOOST_CHECK(!process.poll());
}

BOOST_AUTO_TEST_CASE(move) {
  ya::test::DummyProcess process;
  cg->attachTask(process.pid());
  {
    yac::TerminationGuard tg1(cg);
    BOOST_CHECK(tg1);

    yac::TerminationGuard tg2 = std::move(tg1);
    BOOST_CHECK(!tg1);
    BOOST_CHECK(tg2);

    tg1 = std::move(tg2);
    BOOST_CHECK(tg1);
    BOOST_CHECK(!tg2);

    BOOST_CHECK(!process.poll());
  }
  waitCheck(process);
  BOOST_CHECK_EQUAL(cg->tasks().size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()  // TerminationGuard

BOOST_AUTO_TEST_SUITE_END()  // Terminate

BOOST_FIXTURE_TEST_SUITE(Subsystems, MultipleControlGroupFixture)

BOOST_AUTO_TEST_CASE(CpuAccounting) {
  const yac::CpuAccounting cpuAcct(thisCG);
  BOOST_TEST_MESSAGE("user: " << cpuAcct.userUsage().count());
  BOOST_TEST_MESSAGE("system: " << cpuAcct.systemUsage().count());
  BOOST_TEST_MESSAGE("usage: " << cpuAcct.usage().count());
}

BOOST_AUTO_TEST_SUITE_END()  // Subsystems

BOOST_AUTO_TEST_SUITE_END()  // cgroup_subsystems
