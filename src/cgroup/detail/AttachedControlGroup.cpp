#include <yandex/contest/system/cgroup/detail/AttachedControlGroup.hpp>

#include <yandex/contest/detail/LogHelper.hpp>

#include <bunsan/enable_error_info.hpp>

#include <boost/filesystem/operations.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup{namespace detail
{
    AttachedControlGroup::AttachedControlGroup(
        const SystemInfoPointer &systemInfo,
        const std::size_t hierarchyId,
        const boost::filesystem::path &controlGroup,
        const SingleControlGroupPointer &parent):
        SingleControlGroup(systemInfo, hierarchyId, controlGroup, parent)
    {
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            STREAM_TRACE << "Attempt to attach cgroup object to " << *this << ".";
            if (!boost::filesystem::is_directory(location()))
            {
                STREAM_ERROR << "Unable to attach cgroup object to " <<
                                *this << " (does not exist).";
                BOOST_THROW_EXCEPTION(SingleControlGroupNotExistsError());
            }
            STREAM_TRACE << "Control group object was attached to " << *this << ".";
        }
        BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(
            SingleControlGroupError::hierarchyId(hierarchyId) <<
            SingleControlGroupError::controlGroupPath(controlGroup) <<
            SingleControlGroupError::path(location()))
    }

    void AttachedControlGroup::printSingle(std::ostream &out) const
    {
        out << "\"attached\"";
    }
}}}}}
