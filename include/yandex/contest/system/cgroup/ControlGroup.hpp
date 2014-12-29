#pragma once

#include <yandex/contest/system/cgroup/Error.hpp>
#include <yandex/contest/system/cgroup/Forward.hpp>
#include <yandex/contest/system/cgroup/detail/StreamableWrapper.hpp>

#include <yandex/contest/IntrusivePointeeBase.hpp>

#include <bunsan/stream_enum.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <unordered_set>

#include <sys/types.h>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    struct ControlGroupError: virtual Error {};

    struct ControlGroupFieldError: virtual ControlGroupError
    {
        typedef boost::error_info<
            struct fieldNameTag,
            std::string
        > fieldName;
        typedef boost::error_info<
            struct fieldPathTag,
            boost::filesystem::path
        > fieldPath;
    };

    struct ControlGroupInvalidFieldNameError: virtual ControlGroupFieldError {};
    struct ControlGroupFieldDoesNotExistError: virtual ControlGroupFieldError {};
    struct ControlGroupInvalidFieldFileError: virtual ControlGroupFieldError {};

    /*!
     * \warning Objects of this class are not thread-safe, but reentrant.
     * Static functions guarantee to allocate new instances of ControlGroup --
     * root objects.
     * Root objects may allocate new cgroups by member functions -- child objects.
     * Child objects may allocate new cgroups by member functions or return
     * previously allocated child objects related to root object.
     * It is guaranteed that no member function will return child object
     * related to other root object. So it is safe to use non-intersecting
     * hierarchies of objects in different threads.
     */
    class ControlGroup: public IntrusivePointeeBase
    {
    public:
        typedef std::unordered_set<pid_t> Tasks;
        typedef std::function<void (std::istream &)> Reader;
        typedef std::function<void (std::ostream &)> Writer;

    public:
        /// Attach to existing child cgroup.
        ControlGroupPointer attachChild(
            const boost::filesystem::path &childControlGroup);

        /// Create new child cgroup.
        ControlGroupPointer createChild(
            const boost::filesystem::path &childControlGroup);

        /// Create new child cgroup.
        ControlGroupPointer createChild(
            const boost::filesystem::path &childControlGroup,
            const mode_t mode);

        ControlGroupPointer parent();

    public:
        boost::filesystem::path fieldPath(const std::string &fieldName) const;

        virtual Tasks tasks()=0;

        virtual void attachTask(const pid_t pid)=0;

        virtual bool notifyOnRelease()=0;
        virtual void setNotifyOnRelease(const bool notifyOnRelease=true)=0;

        /*!
         * \note This field is only available in root cgroup.
         *
         * \todo Do I need to implement forwarding?
         */
        virtual std::string releaseAgent()=0;

        /// \copydoc releaseAgent()
        virtual void setReleaseAgent(const std::string &releaseAgent)=0;

        virtual bool cloneChildren()=0;
        virtual void setCloneChildren(const bool cloneChildren=true)=0;

        template <typename T>
        void readField(const std::string &fieldName, T &data)
        {
            readField<const detail::IStreamableWrapper>(
                fieldName,
                detail::IStreamableWrapper(data)
            );
        }

        template <typename T>
        void writeField(const std::string &fieldName, const T &data)
        {
            writeField<detail::OStreamableWrapper>(
                fieldName,
                detail::OStreamableWrapper(data)
            );
        }

        template <typename T>
        T readField(const std::string &fieldName)
        {
            T data;
            readField(fieldName, data);
            return data;
        }

        template <typename T>
        T readFieldAll(const std::string &fieldName)
        {
            return boost::lexical_cast<T>(
                readFieldAll<std::string>(fieldName)
            );
        }

        template <typename T>
        T readFieldAllRtrimmed(const std::string &fieldName)
        {
            return boost::lexical_cast<T>(
                readFieldAllRtrimmed<std::string>(fieldName)
            );
        }

        void readFieldByReader(const std::string &fieldName,
                               const Reader &reader);
        void writeFieldByWriter(const std::string &fieldName,
                                const Writer &writer);

    protected:
        ControlGroup()=default;

        /*!
         * \brief Hypothetical field path.
         *
         * \param fieldName Correct field name
         * (checked by ControlGroup::fieldPath()).
         * Otherwise behavior is undefined.
         *
         * \warning Returned path may not exist or may point to invalid file.
         * This will be checked by ControlGroup::fieldPath().
         */
        virtual boost::filesystem::path fieldPath__(
            const std::string &fieldName) const=0;

        virtual void print(std::ostream &out) const=0;

        friend std::ostream &operator<<(std::ostream &out,
                                        const ControlGroup &cgroup);

        /// Attach to existing child cgroup.
        virtual ControlGroupPointer attachChild__(
            const boost::filesystem::path &childControlGroup)=0;

        /// Create new child cgroup.
        virtual ControlGroupPointer createChild__(
            const boost::filesystem::path &childControlGroup,
            const mode_t mode)=0;

        virtual ControlGroupPointer parent__()=0;
    };

    template <>
    void ControlGroup::readField(
        const std::string &fieldName,
        const detail::IStreamableWrapper &data);

    template <>
    void ControlGroup::writeField(
        const std::string &fieldName,
        const detail::OStreamableWrapper &data);

    template <>
    std::string ControlGroup::readFieldAll(const std::string &fieldName);

    template <>
    std::string ControlGroup::readFieldAllRtrimmed(const std::string &fieldName);
}}}}
