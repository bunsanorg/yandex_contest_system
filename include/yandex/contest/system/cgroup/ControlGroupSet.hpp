#pragma once

#include "yandex/contest/system/cgroup/Error.hpp"
#include "yandex/contest/system/cgroup/ControlGroup.hpp"

#include <unordered_map>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    struct ControlGroupSetError: virtual Error {};

    struct SubsystemIsNotAttachedError: virtual ControlGroupSetError
    {
        typedef boost::error_info<struct subsystemTag, std::string> subsystem;
    };

    class ControlGroupSet
    {
    public:
        ControlGroupSet()=default;

        ControlGroupSet(const ControlGroupSet &)=delete;
        ControlGroupSet &operator=(const ControlGroupSet &)=delete;

        ControlGroupSet(ControlGroupSet &&);
        ControlGroupSet &operator=(ControlGroupSet &&);

        template <typename ... Args>
        explicit ControlGroupSet(ControlGroup &&controlGroup, Args &&...args):
            ControlGroupSet(std::forward<Args>(args)...)
        {
            insert(std::move(controlGroup));
        }

        template <typename Iter>
        void insert(Iter begin, const Iter &end)
        {
            for (; begin != end; ++begin)
                insert(*begin);
        }

        void insert(ControlGroup &&controlGroup);

        void clear();

        void attachTask(const pid_t pid);

        ControlGroupSet createChild();
        ControlGroupSet attachChild();

        template <typename Subsystem>
        Subsystem subsystem()
        {
            const auto iter = groups_.find(Subsystem::SUBSYSTEM_NAME);
            if (iter != groups_.end())
                return Subsystem(iter->second);
            else
                BOOST_THROW_EXCEPTION(SubsystemIsNotAttachedError() <<
                                      SubsystemIsNotAttachedError::subsystem(
                                          Subsystem::SUBSYSTEM_NAME));
        }

        void swap(ControlGroupSet &controlGroupSet) noexcept;

    private:
        std::unordered_map<std::string, ControlGroup> groups_;
    };

    inline void swap(ControlGroupSet &a, ControlGroupSet &b) noexcept
    {
        a.swap(b);
    }
}}}}
