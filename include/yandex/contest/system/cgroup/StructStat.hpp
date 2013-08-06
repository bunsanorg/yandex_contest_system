#pragma once

#include <yandex/contest/system/cgroup/Error.hpp>
#include <yandex/contest/system/cgroup/Stat.hpp>
#include <yandex/contest/system/cgroup/Types.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    struct StatKeyNotFoundError: virtual Error
    {
        typedef boost::error_info<struct keyTag, std::string> key;
    };

    template <typename Config, typename Units_, typename Struct,
              typename Converter=detail::UnitsConverter<Units_>>
    class StructStat: public virtual Stat<Config, Units_, Converter>
    {
    private:
        typedef Stat<Config, Units_, Converter> StatType;

    public:
        typedef typename StatType::Units Units;
        typedef typename StatType::Map Map;

    public:
        Struct stat() const
        {
            const Map map = StatType::stat();
            Struct str;
            InputArchive ia(map);
            ia >> str;
            return str;
        }

    private:
        class InputArchive
        {
        public:
            explicit InputArchive(const Map &map): map_(map) {}

            template <typename T>
            InputArchive &operator&(T &obj)
            {
                return (*this) >> obj;
            }

            template <typename T>
            InputArchive &operator>>(const boost::serialization::nvp<T> &nvp)
            {
                const auto iter = map_.find(nvp.name());
                if (iter == map_.end())
                    BOOST_THROW_EXCEPTION(StatKeyNotFoundError() <<
                                          StatKeyNotFoundError::key(nvp.name()));
                nvp.value() = iter->second;
                return *this;
            }

            template <typename T>
            InputArchive &operator>>(T &obj)
            {
                // FIXME use boost::serialization::access
                obj.serialize(*this, 0);
                return *this;
            }

        private:
            const Map &map_;
        };
    };
}}}}
