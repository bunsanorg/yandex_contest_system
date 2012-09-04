#pragma once

#include "yandex/contest/system/cgroup/Error.hpp"

#include <string>
#include <unordered_set>
#include <utility>

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
        typedef std::unordered_set<std::string> Tasks;

    public:
        ControlGroup()=default;

        ControlGroup(const boost::filesystem::path &root,
                     const boost::filesystem::path &name);

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
            ControlGroup(std::forward<Arg>(arg), std::forward<Args>(args)...).swap(*this);
        }

        void remove() noexcept; // FIXME NOEXCEPT???

        void terminate() noexcept; // FIXME NOEXCEPT???

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
        struct ControlGroupData;

        /// \throws InvalidControlGroupError if control group is not initialized.
        const ControlGroupData &data() const;

    private:
        struct ControlGroupData
        {
            boost::filesystem::path root;
            boost::filesystem::path name;
        };

        boost::optional<ControlGroupData> data_;
    };

    inline void swap(ControlGroup &a, ControlGroup &b) noexcept
    {
        a.swap(b);
    }
}}}}
