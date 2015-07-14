#pragma once

#include <yandex/contest/system/execution/AsyncProcess.hpp>

#include <bunsan/test/environment.hpp>
#include <bunsan/test/exec_test.hpp>

namespace yandex {
namespace contest {
namespace system {
namespace test {

class DummyProcess : public execution::AsyncProcess {
 public:
  DummyProcess() : execution::AsyncProcess(options()) {}

 private:
  static execution::AsyncProcess::Options options() {
    execution::AsyncProcess::Options opt;
    opt.executable =
        bunsan::test::dir::tests::resources::binary() / "dummy_process";
    opt.usePath = false;
    return opt;
  }
};

}  // namespace test
}  // namespace system
}  // namespace contest
}  // namespace yandex
