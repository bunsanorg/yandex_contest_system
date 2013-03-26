#include "yandex/contest/system/cgroup/detail/AttachedControlGroup.hpp"

namespace yandex{namespace contest{namespace system{namespace cgroup{namespace detail
{
    AttachedControlGroup::AttachedControlGroup(const SystemInfoPointer &systemInfo,
                                               const std::size_t hierarchyId,
                                               const boost::filesystem::path &controlGroup,
                                               const SingleControlGroupPointer &parent):
        SingleControlGroup(systemInfo, hierarchyId, controlGroup, parent)
    {
        // TODO check
    }

    void AttachedControlGroup::close()
    {
        // nothing to do
    }

    void AttachedControlGroup::printSingle(std::ostream &out) const
    {
        out << "\"attached\"";
    }
}}}}}
