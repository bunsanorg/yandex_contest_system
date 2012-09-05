#pragma once

#include "yandex/contest/system/cgroup/Error.hpp"
#include "yandex/contest/system/cgroup/MountPoint.hpp"

#include <string>
#include <unordered_set>
#include <utility>
#include <functional>

#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

#include <sys/types.h>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    struct InvalidControlGroupError: Error {};

    /*!
     * \throws InvalidControlGroupError if control group is not initialized.
     */
    class ControlGroup
    {
    public:
        typedef std::unordered_set<pid_t> Tasks;
        typedef std::function<void (std::istream &)> Reader;
        typedef std::function<void (std::ostream &)> Writer;

    public:
        ControlGroup()=default;

        explicit ControlGroup(const boost::filesystem::path &name,
                              const boost::filesystem::path &root=getMountPoint());

        ControlGroup(ControlGroup &&);
        ControlGroup &operator=(ControlGroup &&);

        ~ControlGroup();

        ControlGroup(const ControlGroup &)=delete;
        ControlGroup &operator=(const ControlGroup &)=delete;

        explicit operator bool() const;

        void swap(ControlGroup &controlGroup) noexcept;

        template <typename Arg, typename ... Args>
        void create(Arg &&arg, Args &&...args)
        {
            // default constructor is not allowed
            ControlGroup(std::forward<Arg>(arg),
                         std::forward<Args>(args)...).swap(*this);
        }

        void remove();

        void terminate();

        Tasks tasks();

        void attach(const pid_t pid);

        bool notifyOnRelease();
        void setNotifyOnRelease(const bool notifyOnRelease=true);

        std::string releaseAgent();
        void setReleaseAgent(const std::string &releaseAgent);

        bool cloneChildren();
        void setCloneChildren(const bool cloneChildren=true);

        template <typename T>
        void writeField(const std::string &fieldName, const T &data)
        {
            writeStringField(fieldName, boost::lexical_cast<std::string>(data));
        }

        template <typename T>
        T readField(const std::string &fieldName)
        {
            return boost::lexical_cast<T>(readStringField(fieldName));
        }

        void readFieldByReader(const std::string &fieldName, const Reader &reader);
        void writeFieldByWriter(const std::string &fieldName, const Writer &writer);

    public:
        /// Get control group path.
        boost::filesystem::path path() const;

        /// Get control group name.
        const boost::filesystem::path &name() const;

        /// Get path to control group field.
        boost::filesystem::path field(const std::string &fieldName) const;

    private:
        std::string readStringField(const std::string &fieldName);
        void writeStringField(const std::string &fieldName, const std::string &data);

    private:
        struct ControlGroupData
        {
            boost::filesystem::path root;
            boost::filesystem::path name;
        };

    private:

        /// \throws InvalidControlGroupError if control group is not initialized.
        const ControlGroupData &data() const;

    private:
        boost::optional<ControlGroupData> data_;
    };

    inline void swap(ControlGroup &a, ControlGroup &b) noexcept
    {
        a.swap(b);
    }
}}}}
