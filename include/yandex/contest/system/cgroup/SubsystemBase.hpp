#pragma once

#include "yandex/contest/system/cgroup/ControlGroup.hpp"
#include "yandex/contest/system/cgroup/Subsystem.hpp"

#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    template <typename Config>
    class SubsystemBase
    {
    protected:
        SubsystemBase()=default;

    protected:
        static inline std::string fieldName(const std::string &fieldName_)
        {
            return Config::SUBSYSTEM_NAME + "." + fieldName_;
        }

        boost::filesystem::path field(const std::string &fieldName_) const
        {
            return controlGroup().field(fieldName(fieldName_));
        }

        template <typename T>
        void writeField(const std::string &fieldName_, const T &data) const
        {
            controlGroup().writeField(fieldName(fieldName_), data);
        }

        template <typename T>
        T readField(const std::string &fieldName_) const
        {
            return controlGroup().template readField<T>(fieldName(fieldName_));
        }

    private:
        inline ControlGroup &controlGroup() const
        {
            // TODO Think about such design,
            // I don't like reinterpret_cast here.
            // It is guaranteed that this object
            // is instance of Subsystem<Config>.
            return reinterpret_cast<const Subsystem<Config> *>(this)->controlGroup();
        }
    };
}}}}
