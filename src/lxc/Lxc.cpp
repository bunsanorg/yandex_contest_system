#include <yandex/contest/system/lxc/Lxc.hpp>

#include <yandex/contest/detail/LogHelper.hpp>
#include <yandex/contest/system/execution/ErrCall.hpp>
#include <yandex/contest/system/unistd/Fstab.hpp>

#include <bunsan/filesystem/fstream.hpp>

#include <boost/filesystem/operations.hpp>

namespace yandex{namespace contest{namespace system{namespace lxc
{
    using execution::ProcessArguments;

    Lxc::Lxc(const std::string &name,
             const boost::filesystem::path &dir,
             const Config &config):
        name_(name),
        dir_(boost::filesystem::absolute(dir)),
        rootfs_(dir_ / "rootfs"),
        rootfsMount_(dir_ / "rootfs.mount"),
        configPath_(dir_ / "config")
    {
        STREAM_INFO << "Trying to create \"" << name_ << "\" LXC.";
        Config config_ = config;
        prepare(config_);
        STREAM_INFO << "Trying to create root directory " <<
                       "for \"" << name_ << "\" LXC " <<
                       "at " << rootfs_ << ".";
        boost::filesystem::create_directory(rootfs_);
        STREAM_INFO << "Root directory was successfully created " <<
                       "for \"" << name_ << "\" LXC.";
        STREAM_INFO << "Trying to create root.mount directory " <<
                       "for \"" << name_ << "\" LXC " <<
                       "at " << rootfsMount_ << ".";
        boost::filesystem::create_directory(rootfsMount_);
        STREAM_INFO << "Root directory was successfully created " <<
                       "for \"" << name_ << "\" LXC.";
        {
            STREAM_INFO << "Trying to write lxc.conf(5) " <<
                           "for \"" << name_ << "\" LXC.";
            bunsan::filesystem::ofstream cfg(configPath_);
            BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(cfg)
            {
                cfg << config_;
            }
            BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(cfg)
            cfg.close();
            STREAM_INFO << "lxc.conf(5) was successfully written " <<
                           "for \"" << name << "\" LXC.";
        }
    }

    void Lxc::execute_(
        Executor &executor,
        const execution::AsyncProcess::Options &options)
    {
        // TODO thread-safety
        // TODO lxc-execute errors control
        STREAM_INFO << "Attempt to execute command " <<
                       "in \"" << name_ << "\" LXC.";
        // we need to use it twice
        // and do not want it to change
        executor(transform(options));
        STREAM_INFO << "Command execution is started " <<
                       "in \"" << name_ << "\" LXC.";
    }

    void Lxc::stop()
    {
        STREAM_INFO << "Trying to stop \"" << name_ << "\" LXC.";
        const execution::Result result =
            execution::getErrCallArgv("lxc-stop", "-n", name_, "--kill");
        if (result)
        {
            STREAM_INFO << "\"" << name_ << "\" LXC " <<
                           "was successfully stopped.";
        }
        else if (result.exitStatus == 2)
        {
            STREAM_INFO << "\"" << name_ << "\" LXC " <<
                           "was already stopped.";
        }
        else
        {
            STREAM_ERROR << "Error while stopping \"" << name_ <<
                            "\" LXC: \"" << result.err << "\", " <<
                            "exception is thrown.";
            BOOST_THROW_EXCEPTION(
                toUtilityError(result) <<
                Error::message("Error while stopping LXC."));
        }
    }

    Lxc::~Lxc()
    {
        STREAM_INFO << "Trying to remove \"" << name_ << "\" LXC.";
        try
        {
            stop();
        }
        catch (std::exception &e)
        {
            STREAM_ERROR << "Unable to stop \"" <<
                            name_ << "\" LXC (ignoring).";
        }
        boost::system::error_code ec;
        boost::filesystem::remove_all(dir_, ec);
        if (ec)
            STREAM_ERROR << "Unable to remove directory " << dir_ << " " <<
                            "with \"" << name_ << "\" LXC " <<
                            "due to \"" << ec << "\" (ignoring).";
        else
            STREAM_INFO << "\"" << name_ << "\" LXC " <<
                           "was successfully removed.";
    }

    const boost::filesystem::path &Lxc::rootfs() const
    {
        return rootfs_;
    }

    void Lxc::prepare(Config &config)
    {
        config.rootfs = RootfsConfig{
            .fsname = rootfs_,
            .mount = rootfsMount_
        };
        if (config.mount)
        {
            if (config.mount->fstab)
            {
                unistd::Fstab fstab;
                fstab.load(config.mount->fstab.get());
                for (unistd::MountEntry &entry: fstab)
                    prepare(entry);
                const boost::filesystem::path fstab_ = dir_ / "fstab";
                fstab.save(fstab_);
                config.mount->fstab = fstab_;
            }
            if (config.mount->entries)
            {
                for (unistd::MountEntry &entry: config.mount->entries.get())
                    prepare(entry);
            }
        }
    }

    void Lxc::prepare(unistd::MountEntry &entry)
    {
        const boost::filesystem::path dst = rootfs_ / entry.dir;
        boost::filesystem::create_directories(dst);
        entry.dir = dst.string();
    }

    execution::AsyncProcess::Options Lxc::transform(
        const execution::AsyncProcess::Options &options) const
    {
        execution::AsyncProcess::Options opts = options;
        opts.executable = "lxc-execute";
        opts.usePath = true;
        // options.arguments[1:]
        const ProcessArguments arguments(
            options.arguments.begin() + !options.arguments.empty(),
            options.arguments.end()
        );
        opts.arguments = execution::collect(
            "lxc-execute",
            "-n", name_,
            "-f", configPath_,
            "--", options.executable.string(), arguments
        );
        return opts;
    }

    UtilityError Lxc::toUtilityError(const execution::Result &result) const
    {
        return UtilityError(result) << Error::name(name_);
    }
}}}}
