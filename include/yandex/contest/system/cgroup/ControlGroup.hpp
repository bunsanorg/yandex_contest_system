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
    struct ControlGroupError: virtual Error {};
    struct InvalidControlGroupError: virtual ControlGroupError {};
    struct MultipleControlGroupsError: virtual ControlGroupError {};

    /*!
     * \throws InvalidControlGroupError if control group is not initialized.
     */
    class ControlGroup
    {
    public:
        typedef std::unordered_set<pid_t> Tasks;
        typedef std::function<void (std::istream &)> Reader;
        typedef std::function<void (std::ostream &)> Writer;

        enum AttachType { Attach };

    public:
        ControlGroup()=default;

        /// Create not-owning ControlGroup object.
        ControlGroup(const boost::filesystem::path &name, const AttachType,
                     const boost::filesystem::path &root=getMountPoint());

        ControlGroup(const boost::filesystem::path &name, const mode_t mode,
                     const boost::filesystem::path &root=getMountPoint());

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

        /// \warning Required freezer subsystem mounted.
        void terminate();

        template <typename Arg, typename ... Args>
        void attach(Arg &&arg, Args &&...args)
        {
            create(std::forward<Arg>(arg), Attach, std::forward<Args>(args)...);
        }

        /// Make object invalid, but do not remove cgroup.
        void detach();

        /// Terminate all processes and remove this group.
        void remove();

        /*!
         * \brief If ControlGroup owns cgroup, call remove() else detach().
         *
         * Does nothing if object is not valid.
         */
        void close();

        /// Get parent ControlGroup (not-owning).
        ControlGroup parent() const;

        /// Attach to existing child cgroup.
        ControlGroup attachChild(const boost::filesystem::path &name) const;

        /// Create new child cgroup.
        ControlGroup createChild(const boost::filesystem::path &name, const mode_t mode) const;

        /// \copydoc createChild()
        ControlGroup createChild(const boost::filesystem::path &name) const;

        Tasks tasks();

        void attachTask(const pid_t pid);

        bool notifyOnRelease();
        void setNotifyOnRelease(const bool notifyOnRelease=true);

        /*!
         * \note This field is only available in root cgroup.
         *
         * \todo Do I need to implement forwarding?
         */
        std::string releaseAgent();

        /// \copydoc releaseAgent()
        void setReleaseAgent(const std::string &releaseAgent);

        bool cloneChildren();
        void setCloneChildren(const bool cloneChildren=true);

        template <typename T>
        void writeField(const std::string &fieldName, const T &data)
        {
            writeField<std::string>(fieldName, boost::lexical_cast<std::string>(data));
        }

        template <typename T>
        T readField(const std::string &fieldName)
        {
            return boost::lexical_cast<T>(readField<std::string>(fieldName));
        }

        void readFieldByReader(const std::string &fieldName, const Reader &reader);
        void writeFieldByWriter(const std::string &fieldName, const Writer &writer);

    public:
        boost::filesystem::path path() const;

        const boost::filesystem::path &name() const;

        const boost::filesystem::path &root() const;

        std::size_t hierarchy() const;

        boost::filesystem::path field(const std::string &fieldName) const;

    public:
        /*!
         * \brief Get cgroup owning specified pid.
         *
         * \throw MultipleControlGroupsError If process is owned by multiple cgroups.
         */
        static ControlGroup getControlGroup(const pid_t pid);

        /// Get cgroups owning specified pid.
        static std::vector<ControlGroup> getControlGroups(const pid_t pid);

    private:
        struct ControlGroupData
        {
            boost::filesystem::path root;
            boost::filesystem::path name;
            std::size_t hierarchy;
            bool own;
        };

        friend std::ostream &operator<<(std::ostream &out,
                                        const ControlGroupData &data);

        friend std::ostream &operator<<(std::ostream &out,
                                        const boost::optional<ControlGroupData> &data);

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

    template <>
    void ControlGroup::writeField(const std::string &fieldName, const std::string &data);

    template <>
    std::string ControlGroup::readField(const std::string &fieldName);
}}}}
