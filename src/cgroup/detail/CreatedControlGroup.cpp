#include <yandex/contest/system/cgroup/detail/CreatedControlGroup.hpp>
#include <yandex/contest/system/unistd/Operations.hpp>

#include <yandex/contest/detail/LogHelper.hpp>

#include <bunsan/enable_error_info.hpp>

#include <boost/filesystem/operations.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup{namespace detail
{
    CreatedControlGroup::CreatedControlGroup(const SystemInfoPointer &systemInfo,
                                             const std::size_t hierarchyId,
                                             const boost::filesystem::path &controlGroup,
                                             const mode_t mode,
                                             const SingleControlGroupPointer &parent):
        SingleControlGroup(systemInfo, hierarchyId, controlGroup, parent)
    {
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            STREAM_TRACE << "Attempt to create cgroup at " << *this << ".";
            if (!unistd::create_directory(location(), mode))
            {
                STREAM_ERROR << "Unable to create cgroup at " << *this << " (already exists).";
                BOOST_THROW_EXCEPTION(SingleControlGroupExistsError());
            }
            STREAM_TRACE << "Control group at " << *this << " was created.";
        }
        BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(
            SingleControlGroupError::hierarchyId(hierarchyId) <<
            SingleControlGroupError::controlGroupPath(controlGroup) <<
            SingleControlGroupError::path(location()))
    }

    CreatedControlGroup::~CreatedControlGroup()
    {
        try
        {
            BUNSAN_EXCEPTIONS_WRAP_BEGIN()
            {
                STREAM_TRACE << "Attempt to remove cgroup = " << *this << ".";
                if (!boost::filesystem::remove(location()))
                {
                    STREAM_ERROR << "Unable to remove cgroup = " << *this << " (does not exist).";
                }
                STREAM_TRACE << "Control group " << *this << " was successfully removed.";
            }
            BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(
                SingleControlGroupError::hierarchyId(hierarchyId()) <<
                SingleControlGroupError::controlGroupPath(controlGroup()) <<
                SingleControlGroupError::path(location()))
        }
        catch (std::exception &e)
        {
            STREAM_ERROR << "Unable to successfully destroy CreatedControlGroup " <<
                            *this << " due to: " << e.what();
        }
        catch (...)
        {
            STREAM_ERROR << "Unable to successfully destroy CreatedControlGroup " <<
                            *this << " due to unknown exception.";
        }
    }

    void CreatedControlGroup::printSingle(std::ostream &out) const
    {
        out << "\"created\"";
    }
}}}}}
