#pragma once

#include <yandex/contest/system/cgroup/ControlGroup.hpp>
#include <yandex/contest/system/cgroup/Subsystem.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

#include <string>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    template <typename Config>
    class SubsystemBase
    {
    public:
        virtual ControlGroup &controlGroup() const=0;

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

        void readFieldByReader(const std::string &fieldName_,
                               const ControlGroup::Reader &reader) const
        {
            controlGroup().readFieldByReader(fieldName(fieldName_), reader);
        }

        void writeFieldByWriter(const std::string &fieldName_,
                                const ControlGroup::Writer &writer) const
        {
            controlGroup().writeFieldByWriter(fieldName(fieldName_), writer);
        }
    };
}}}}
