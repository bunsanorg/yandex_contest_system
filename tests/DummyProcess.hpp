#pragma once

#include <yandex/contest/system/execution/AsyncProcess.hpp>

#include <bunsan/testing/environment.hpp>
#include <bunsan/testing/exec_test.hpp>

namespace yandex{namespace contest{namespace system{namespace testing
{
    class DummyProcess: public execution::AsyncProcess
    {
    public:
        DummyProcess(): execution::AsyncProcess(options()) {}

    private:
        static execution::AsyncProcess::Options options()
        {
            execution::AsyncProcess::Options opt;
            opt.executable =
                bunsan::testing::dir::tests::resources::binary() /
                "dummy_process";
            opt.usePath = false;
            return opt;
        }
    };
}}}}
