#include "yandex/contest/system/cgroup/detail/CreatedControlGroup.hpp"

namespace yandex{namespace contest{namespace system{namespace cgroup{namespace detail
{
    CreatedControlGroup::CreatedControlGroup(const SystemInfoPointer &systemInfo,
                                             const std::size_t hierarchyId,
                                             const boost::filesystem::path &controlGroup,
                                             const mode_t mode,
                                             const SingleControlGroupPointer &parent):
        SingleControlGroup(systemInfo, hierarchyId, controlGroup, parent)
    {
        // TODO create
    }

    CreatedControlGroup::~CreatedControlGroup()
    {
        try
        {
            close();
        }
        catch (std::exception &e)
        {
            // TODO
        }
        catch (...)
        {
            // TODO
        }
    }

    void CreatedControlGroup::close()
    {
        // TODO
    }

    void CreatedControlGroup::printSingle(std::ostream &out) const
    {
        out << "\"created\"";
    }
}}}}}
