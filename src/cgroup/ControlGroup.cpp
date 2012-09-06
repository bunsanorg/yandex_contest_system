#include "yandex/contest/system/cgroup/ControlGroup.hpp"
#include "yandex/contest/system/cgroup/Freezer.hpp"

#include "yandex/contest/system/unistd/Operations.hpp"

#include "yandex/contest/SystemError.hpp"

#include "yandex/contest/detail/LogHelper.hpp"

#include <iterator>
#include <sstream>
#include <thread>
#include <chrono>

#include <csignal>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    std::ostream &operator<<(std::ostream &out, const ControlGroup::ControlGroupData &data)
    {
        out << "{ ControlGroup: name = " << data.name <<
               "; root = " << data.root << "; " <<
               (data.own ? "own" : "does not own") << " }";
        return out;
    }

    std::ostream &operator<<(
        std::ostream &out, const boost::optional<ControlGroup::ControlGroupData> &data)
    {
        if (data)
            out << data.get();
        else
            out << "{ uninitialized ControlGroup }";
        return out;
    }

    ControlGroup::ControlGroup(const boost::filesystem::path &name,
                               const AttachType,
                               const boost::filesystem::path &root):
        data_(ControlGroupData{root, name, false})
    {
        STREAM_DEBUG << data_ << " was attached.";
    }

    ControlGroup::ControlGroup(const boost::filesystem::path &name,
                               const mode_t mode,
                               const boost::filesystem::path &root):
        data_(ControlGroupData{root, name, true})
    {
        STREAM_DEBUG << "Attempt to create new " << data_ << ".";
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
        controlGroup.close();
    }

    ControlGroup &ControlGroup::operator=(ControlGroup &&controlGroup)
    {
        swap(controlGroup);
        controlGroup.close();
        return *this;
    }

    ControlGroup::~ControlGroup()
    {
        try
        {
            close();
        }
        catch (std::exception &e)
        {
            if (data_)
            {
                STREAM_ERROR << data_ << " was not closed due to error: \"" <<
                                e.what() << "\" (ignoring).";
            }
            else
            {
                STREAM_ERROR << "Unexpected error in uninitialized ControlGroup: \"" <<
                                e.what() << "\" (ignoring).";
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
        STREAM_DEBUG << "Attempt to remove " << data() << ".";
        terminate();
        // Some processes may not terminate at this moment,
        // we need to wait for them.
        // TODO Another solution it to move them to the parent cgroup
        // but I am not sure how to implement it robust.
        while (!tasks().empty())
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // FIXME hardcode
        unistd::rmdir(path());
        STREAM_DEBUG << data_ << " was successfully removed.";
        data_.reset();
    }

    void ControlGroup::detach()
    {
        STREAM_DEBUG << "Attempt to detach " << data_ << ".";
        data_.reset();
    }

    void ControlGroup::close()
    {
        if (data_)
        {
            STREAM_DEBUG << "Attempt to close " << data_ << ".";
            if (data_->own)
                remove();
            else
                detach();
        }
    }

    ControlGroup ControlGroup::parent() const
    {
        return ControlGroup(name().parent_path(), Attach, root());
    }

    ControlGroup ControlGroup::attachChild(const boost::filesystem::path &name_) const
    {
        return ControlGroup(name() / name_, Attach, root());
    }

    ControlGroup ControlGroup::createChild(const boost::filesystem::path &name_,
                                           const mode_t mode) const
    {
        return ControlGroup(name() / name_, mode, root());
    }

    ControlGroup ControlGroup::createChild(const boost::filesystem::path &name_) const
    {
        return createChild(name_, 0777);
    }

    void ControlGroup::terminate()
    {
        STREAM_DEBUG << "Attempt to terminate all running processes in " << data() << ".";
        const Freezer freezer(*this);
        freezer.freeze();
        STREAM_DEBUG << "Attempt to kill all frozen processes in " << data() << ".";
        for (const pid_t pid: tasks())
            unistd::kill(pid, SIGKILL);
        freezer.unfreeze();
        // These tasks must somehow be terminated by OS.
        // SIGKILL may not be ignored.
        // User may wait for them (if needed).
        STREAM_DEBUG << "All tasks from " << data() << " were successfully terminated.";
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

    void ControlGroup::attachTask(const pid_t pid)
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
        return root() / name();
    }

    const boost::filesystem::path &ControlGroup::name() const
    {
        return data().name;
    }

    const boost::filesystem::path &ControlGroup::root() const
    {
        return data().root;
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

    ControlGroup ControlGroup::getControlGroup(const pid_t pid)
    {
        auto cgroups = getControlGroups(pid);
        if (cgroups.size() > 1)
            BOOST_THROW_EXCEPTION(MultipleControlGroupsError());
        BOOST_ASSERT(!cgroups.empty());
        return std::move(cgroups.front());
    }

    std::vector<ControlGroup> ControlGroup::getControlGroups(const pid_t pid)
    {
        std::vector<ControlGroup> cgroups;
        std::ostringstream cgroupInfo;
        cgroupInfo << "/proc/" << pid << "/cgroup";
        boost::filesystem::ifstream fin(cgroupInfo.str());
        if (!fin)
            BOOST_THROW_EXCEPTION(SystemError("open"));
        std::string line;
        while (std::getline(fin, line))
        {
            std::vector<std::string> fields;
            std::size_t pos1 = line.find(':');
            BOOST_ASSERT(pos1 != std::string::npos);
            //const std::string hierId = line.substr(0, pos1);
            std::size_t pos2 = line.find(':', pos1 + 1);
            BOOST_ASSERT(pos2 != std::string::npos);
            //const std::string subsystems = line.substr(pos1 + 1, pos2);
            const std::string name = line.substr(pos2 + 1);
            cgroups.push_back(ControlGroup(name, Attach));
        }
        return cgroups;
    }
}}}}
