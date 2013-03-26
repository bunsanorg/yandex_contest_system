#pragma once

#include "yandex/contest/system/cgroup/SingleControlGroup.hpp"

namespace yandex{namespace contest{namespace system{namespace cgroup{namespace detail
{
    class AttachedControlGroup: public SingleControlGroup
    {
    public:
        AttachedControlGroup(const SystemInfoPointer &systemInfo,
                             const std::size_t hierarchyId,
                             const boost::filesystem::path &controlGroup,
                             const SingleControlGroupPointer &parent);

        void close() override;

    protected:
        void printSingle(std::ostream &out) const override;
    };
}}}}}
