#pragma once

#include "yandex/contest/system/lxc/Config.hpp"
#include "yandex/contest/system/lxc/State.hpp"
#include "yandex/contest/system/lxc/Error.hpp"

#include "yandex/contest/StreamEnum.hpp"

#include "yandex/contest/system/execution/Result.hpp"
#include "yandex/contest/system/execution/ResultError.hpp"
#include "yandex/contest/system/execution/AsyncProcess.hpp"

#include "yandex/contest/system/unistd/Pipe.hpp"

#include <string>
#include <vector>
#include <utility>

#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>

namespace yandex{namespace contest{namespace system{namespace lxc
{
    class LXC: private boost::noncopyable
    {
    public:
        typedef lxc_detail::State State;

    public:
        LXC(const std::string &name,
            const boost::filesystem::path &dir,
            const Config &settings);

        void freeze();
        void unfreeze();

        template <typename Ctor>
        auto execute(const Ctor &ctor, const execution::AsyncProcess::Options &options) -> decltype(ctor(options))
        {
            typedef decltype(ctor(options)) result_type;
            struct ExecutorLocal: Executor
            {
                explicit ExecutorLocal(const Ctor &ctor): ctor_(ctor) {}

                void operator()(const execution::AsyncProcess::Options &options) override
                {
                    result = ctor_(options);
                }

                const Ctor &ctor_;
                result_type result;
            } executor(ctor);
            execute_(executor, options);
            return std::move(executor.result);
        }

        /// \todo Is not implemented.
        //void start(const ProcessArguments &arguments);

        /// \todo Is not implemented.
        //void start();

        /// Kill all processes running in container.
        void stop();

        /// Container's state.
        State state();

        ~LXC();

        const boost::filesystem::path &rootfs() const;

    private:
        class Executor
        {
        public:
            virtual void operator()(const execution::AsyncProcess::Options &options)=0;
        };

    private:
        void execute_(Executor &executor, const execution::AsyncProcess::Options &options);

        void prepare(Config &config);
        void prepare(unistd::MountEntry &entry);

        /// Transform process options to make them execute process in container.
        execution::AsyncProcess::Options transform(
            const execution::AsyncProcess::Options &options) const;

        UtilityError toUtilityError(const execution::Result &result) const;

    private:
        const std::string name_;
        const boost::filesystem::path dir_;
        const boost::filesystem::path rootfs_;
        const boost::filesystem::path configPath_;
    };
}}}}
