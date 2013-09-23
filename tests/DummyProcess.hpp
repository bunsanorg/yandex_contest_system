#pragma once

#include <yandex/contest/system/execution/AsyncProcess.hpp>

#include <bunsan/testing/environment.hpp>
#include <bunsan/testing/exec_test.hpp>

namespace yandex{namespace contest{namespace system{namespace testing
{
    class DummyProcess
    {
    public:
        DummyProcess(): process_(options()) {}

        void stop()
        {
            process_.stop();
        }

        execution::AsyncProcess::Pid pid() const
        {
            return process_.pid();
        }

    private:
        static execution::AsyncProcess::Options options()
        {
            execution::AsyncProcess::Options opt;
            opt.executable = bunsan::testing::dir::tests::resources::binary() / "dummy_process";
            opt.usePath = false;
            return opt;
        }

    private:
        execution::AsyncProcess process_;
    };
}}}}
