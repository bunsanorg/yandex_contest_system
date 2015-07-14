#pragma once

#include <yandex/contest/system/cgroup/Error.hpp>
#include <yandex/contest/system/cgroup/Stat.hpp>
#include <yandex/contest/system/cgroup/Types.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {

struct StatKeyNotFoundError : virtual Error {
  using key = boost::error_info<struct keyTag, std::string>;
};

template <typename Config, typename Units_, typename Struct,
          typename Converter = detail::UnitsConverter<Units_>>
class StructStat : public virtual Stat<Config, Units_, Converter> {
 private:
  using StatType = Stat<Config, Units_, Converter>;

 public:
  using Units = typename StatType::Units;
  using Map = typename StatType::Map;

 public:
  Struct stat() const {
    const Map map = StatType::stat();
    Struct str;
    InputArchive ia(map);
    ia >> str;
    return str;
  }

 private:
  class InputArchive {
   public:
    explicit InputArchive(const Map &map) : map_(map) {}

    template <typename T>
    InputArchive &operator&(T &obj) {
      return (*this) >> obj;
    }

    template <typename T>
    InputArchive &operator>>(const boost::serialization::nvp<T> &nvp) {
      const auto iter = map_.find(nvp.name());
      if (iter == map_.end())
        BOOST_THROW_EXCEPTION(StatKeyNotFoundError()
                              << StatKeyNotFoundError::key(nvp.name()));
      nvp.value() = iter->second;
      return *this;
    }

    template <typename T>
    InputArchive &operator>>(T &obj) {
      // FIXME use boost::serialization::access
      obj.serialize(*this, 0);
      return *this;
    }

   private:
    const Map &map_;
  };
};

}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
