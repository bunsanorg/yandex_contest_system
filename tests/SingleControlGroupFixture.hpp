#pragma once

#include "yandex/contest/system/cgroup/SingleControlGroup.hpp"
#include "yandex/contest/system/cgroup/SystemInfo.hpp"
#include "yandex/contest/system/cgroup/ProcessInfo.hpp"

#include "yandex/contest/system/unistd/Operations.hpp"

#include <boost/filesystem/operations.hpp>

namespace ya = yandex::contest::system;
namespace yac = ya::cgroup;
namespace unistd = ya::unistd;

struct SingleControlGroupFixture
{
    SingleControlGroupFixture():
        pid(unistd::getpid()),
        processHierarchy(yac::ProcessInfo::forPid(pid).bySubsystem("freezer")),
        thisCG(yac::SingleControlGroup::forProcessHierarchyInfo(processHierarchy)),
        cg(thisCG->createChild(boost::filesystem::unique_path()))
    {
    }

    ~SingleControlGroupFixture()
    {
        thisCG->attachTask(pid);
    }

    const pid_t pid;
    const yac::ProcessHierarchyInfo processHierarchy;
    const yac::SingleControlGroupPointer thisCG;
    const yac::SingleControlGroupPointer cg;
};
