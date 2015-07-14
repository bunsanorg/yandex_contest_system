#pragma once

#include <boost/noncopyable.hpp>

#include <iostream>
#include <memory>
#include <type_traits>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {
namespace detail {

#define YANDEX_CONTEST_SYSTEM_CGROUP_DETAIL_STREAMABLE_WRAPPER(PREFIX, STREAM, \
                                                               OPERATOR)       \
  class PREFIX##StreamableWrapper {                                            \
   public:                                                                     \
    PREFIX##StreamableWrapper(const PREFIX##StreamableWrapper &) = default;    \
    PREFIX##StreamableWrapper(PREFIX##StreamableWrapper &&) = default;         \
    PREFIX##StreamableWrapper &operator=(const PREFIX##StreamableWrapper &) =  \
        default;                                                               \
    PREFIX##StreamableWrapper &operator=(PREFIX##StreamableWrapper &&) =       \
        default;                                                               \
                                                                               \
    template <typename T>                                                      \
    explicit PREFIX##StreamableWrapper(T &&object)                             \
        : impl_(new Impl<typename std::remove_reference<T>::type>(&object)) {} \
                                                                               \
   private:                                                                    \
    class ImplBase {                                                           \
     public:                                                                   \
      virtual ~ImplBase();                                                     \
      virtual void dispatch(STREAM &stream) = 0;                               \
    };                                                                         \
                                                                               \
    template <typename T>                                                      \
    class Impl : public ImplBase {                                             \
     public:                                                                   \
      explicit Impl(T *object) : object_(object) {}                            \
                                                                               \
      void dispatch(STREAM &stream) override { stream OPERATOR *object_; }     \
                                                                               \
     private:                                                                  \
      T *object_;                                                              \
    };                                                                         \
                                                                               \
    friend STREAM &operator OPERATOR(                                          \
        STREAM &stream, const PREFIX##StreamableWrapper &streamable);          \
                                                                               \
   private:                                                                    \
    std::shared_ptr<ImplBase> impl_;                                           \
  };                                                                           \
                                                                               \
  STREAM &operator OPERATOR(STREAM &stream,                                    \
                            const PREFIX##StreamableWrapper &streamable);

YANDEX_CONTEST_SYSTEM_CGROUP_DETAIL_STREAMABLE_WRAPPER(I, std::istream, >> )
YANDEX_CONTEST_SYSTEM_CGROUP_DETAIL_STREAMABLE_WRAPPER(O, std::ostream, << )

#undef YANDEX_CONTEST_SYSTEM_CGROUP_DETAIL_STREAMABLE_WRAPPER

}  // namespace detail
}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
