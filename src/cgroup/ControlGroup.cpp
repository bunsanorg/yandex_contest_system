#include "yandex/contest/system/cgroup/ControlGroup.hpp"
#include "yandex/contest/system/cgroup/Freezer.hpp"

#include "yandex/contest/system/unistd/Operations.hpp"

#include "yandex/contest/SystemError.hpp"

#include "yandex/contest/detail/LogHelper.hpp"

#include <iterator>

#include <csignal>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    ControlGroup::ControlGroup(const boost::filesystem::path &name,
                               const mode_t mode,
                               const boost::filesystem::path &root):
        data_(ControlGroupData{root, name})
    {
        STREAM_DEBUG << "Attempt to create new control group at " <<
                        root << " named " << name << ".";
        unistd::mkdir(root / name, mode);
        STREAM_DEBUG << "Control group " << name <<
                        " was successfully created at " << root;
    }

    ControlGroup::ControlGroup(const boost::filesystem::path &name,
                               const boost::filesystem::path &root):
        ControlGroup::ControlGroup(name, 0777, root) {}

    ControlGroup::ControlGroup(ControlGroup &&controlGroup)
    {
        swap(controlGroup);
        controlGroup.remove();
    }

    ControlGroup &ControlGroup::operator=(ControlGroup &&controlGroup)
    {
        swap(controlGroup);
        controlGroup.remove();
        return *this;
    }

    ControlGroup::~ControlGroup()
    {
        try
        {
            remove();
        }
        catch (std::exception &e)
        {
            if (data_)
            {
                STREAM_ERROR << "Control group " << data_->name <<
                                " was not removed from " << data_->root <<
                                " due to error: \"" << e.what() <<
                                "\" (ignoring).";
            }
            else
            {
                STREAM_ERROR << "Unexpected error in uninitialized"
                                " control group object: \"" << e.what() << "\".";
            }
        }
        catch (...)
        {
            STREAM_ERROR << "Unknown error (ignoring).";
        }
    }

    ControlGroup::operator bool() const
    {
        return data_;
    }

    void ControlGroup::swap(ControlGroup &controlGroup) noexcept
    {
        using boost::swap;
        swap(data_, controlGroup.data_);
    }

    void ControlGroup::remove()
    {
        if (data_)
        {
            STREAM_DEBUG << "Attempt to remove control group " <<
                            data_->name << " at " << data_->root << ".";
            terminate();
            // TODO some processes may not terminate for this moment
            unistd::rmdir(data_->root / data_->name);
            STREAM_DEBUG << "Control group " << data_->name <<
                            " was successfully removed from " << data_->root;
            data_.reset();
        }
    }

    void ControlGroup::terminate()
    {
        STREAM_DEBUG << "Attempt to terminate all running processes in " <<
                        data().name << " control group.";
        const Freezer freezer(*this);
        freezer.freeze();
        STREAM_DEBUG << "Attempt to kill all frozen processes in " <<
                        data().name << " control group.";
        for (const pid_t pid: tasks())
            unistd::kill(pid, SIGKILL);
        freezer.unfreeze();
        // These tasks must somehow be terminated by OS.
        // SIGKILL may not be ignored.
        // User may wait for them (if needed).
        STREAM_DEBUG << "All tasks from " << data().name <<
                        " control group were successfully terminated.";
    }

    ControlGroup::Tasks ControlGroup::tasks()
    {
        Tasks tasks_;
        readFieldByReader("tasks",
            [&tasks_](std::istream &in)
            {
                tasks_.insert(std::istream_iterator<pid_t>(in),
                              std::istream_iterator<pid_t>());
            });
        return tasks_;
    }

    void ControlGroup::attach(const pid_t pid)
    {
        writeField("tasks", pid);
    }

    bool ControlGroup::notifyOnRelease()
    {
        return readField<int>("notify_on_release");
    }

    void ControlGroup::setNotifyOnRelease(const bool notifyOnRelease)
    {
        writeField<int>("notify_on_release", notifyOnRelease);
    }

    std::string ControlGroup::releaseAgent()
    {
        return readField<std::string>("release_agent");
    }

    void ControlGroup::setReleaseAgent(const std::string &releaseAgent)
    {
        writeField("release_agent", releaseAgent);
    }

    bool ControlGroup::cloneChildren()
    {
        return readField<int>("cgroup.clone_children");
    }

    void ControlGroup::setCloneChildren(const bool cloneChildren)
    {
        return writeField<int>("cgroup.clone_children", cloneChildren);
    }

    boost::filesystem::path ControlGroup::path() const
    {
        return data().root / data().name;
    }

    const boost::filesystem::path &ControlGroup::name() const
    {
        return data().name;
    }

    boost::filesystem::path ControlGroup::field(const std::string &fieldName) const
    {
        return path() / fieldName;
    }

    void ControlGroup::readFieldByReader(const std::string &fieldName, const Reader &reader)
    {
        boost::filesystem::ifstream fin(field(fieldName));
        if (!fin)
            BOOST_THROW_EXCEPTION(SystemError());
        reader(fin);
        fin.close();
        if (fin.bad())
            BOOST_THROW_EXCEPTION(SystemError());
    }

    void ControlGroup::writeFieldByWriter(const std::string &fieldName, const Writer &writer)
    {
        boost::filesystem::ofstream fout(field(fieldName));
        if (!fout)
            BOOST_THROW_EXCEPTION(SystemError());
        writer(fout);
        fout.close();
        if (!fout)
            BOOST_THROW_EXCEPTION(SystemError());
    }

    template <>
    std::string ControlGroup::readField(const std::string &fieldName)
    {
        std::string data;
        readFieldByReader(fieldName,
            [&data](std::istream &in)
            {
                data.assign(std::istreambuf_iterator<char>(in),
                            std::istreambuf_iterator<char>());
            });
        boost::algorithm::trim_right(data);
        return data;
    }

    template <>
    void ControlGroup::writeField(const std::string &fieldName, const std::string &data)
    {
        writeFieldByWriter(fieldName, [&data](std::ostream &out){out << data;});
    }

    const ControlGroup::ControlGroupData &ControlGroup::data() const
    {
        if (data_)
            return data_.get();
        else
            BOOST_THROW_EXCEPTION(InvalidControlGroupError());
    }
}}}}
