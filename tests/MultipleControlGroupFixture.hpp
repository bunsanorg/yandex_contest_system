#pragma once

#include <yandex/contest/system/cgroup/MultipleControlGroup.hpp>

#include <yandex/contest/system/unistd/Operations.hpp>

#include <boost/filesystem/operations.hpp>

namespace ya = yandex::contest::system;
namespace yac = ya::cgroup;
namespace unistd = ya::unistd;

struct MultipleControlGroupFixture
{
    MultipleControlGroupFixture():
        pid(unistd::getpid()),
        thisCG(yac::MultipleControlGroup::forSelf()),
        cgName(boost::filesystem::unique_path()),
        cg(thisCG->createChild(cgName))
    {
    }

    ~MultipleControlGroupFixture()
    {
        thisCG->attachTask(pid);
    }

    const pid_t pid;
    const yac::MultipleControlGroupPointer thisCG;
    const boost::filesystem::path cgName;
    const yac::MultipleControlGroupPointer cg;
};
