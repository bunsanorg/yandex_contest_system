#include "yandex/contest/system/cgroup/ControlGroup.hpp"

#include "yandex/contest/SystemError.hpp"

#include <iterator>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    ControlGroup::ControlGroup(const boost::filesystem::path &root,
                               const boost::filesystem::path &name):
        data_(ControlGroupData{root, name})
    {
        // TODO unistd::mkdir(root / name);
    }

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
        remove();
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

    void ControlGroup::remove() noexcept
    {
        if (data_)
        {
            terminate();
            // TODO unistd::rmdir()
        }
    }

    void ControlGroup::terminate() noexcept
    {
        //freeze();
    }

    ControlGroup::Tasks ControlGroup::tasks()
    {
        Tasks tasks_;
        std::string seq = readField<std::string>("tasks");
        boost::algorithm::split(tasks_, seq,
                                boost::algorithm::is_space(),
                                boost::algorithm::token_compress_on);
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

    std::string ControlGroup::readStringField(const std::string &fieldName)
    {
        boost::filesystem::ifstream fin(field(fieldName));
        if (!fin)
            BOOST_THROW_EXCEPTION(SystemError());
        const std::string buf{
            std::istreambuf_iterator<char>(fin),
            std::istreambuf_iterator<char>()
        };
        fin.close();
        if (!fin)
            BOOST_THROW_EXCEPTION(SystemError());
        return buf;
    }

    void ControlGroup::writeStringField(const std::string &fieldName, const std::string &data)
    {
        boost::filesystem::ofstream fout(field(fieldName));
        if (!fout)
            BOOST_THROW_EXCEPTION(SystemError());
        fout << data;
        fout.close();
        if (!fout)
            BOOST_THROW_EXCEPTION(SystemError());
    }

    const ControlGroup::ControlGroupData &ControlGroup::data() const
    {
        if (data_)
            return data_.get();
        else
            BOOST_THROW_EXCEPTION(InvalidControlGroupError());
    }
}}}}
