#include <yandex/contest/system/lxc/LxcApi.hpp>

#include <yandex/contest/system/lxc/Error.hpp>

#include <lxc/lxccontainer.h>

#include <boost/assert.hpp>

void intrusive_ptr_add_ref(::lxc_container *container) noexcept
{
    BOOST_VERIFY(::lxc_container_get(container));
}

void intrusive_ptr_release(::lxc_container *container) noexcept
{
    BOOST_VERIFY(::lxc_container_put(container) >= 0);
}

namespace yandex{namespace contest{namespace system{namespace lxc{
    namespace api
{
    container_ptr container_new(
        const std::string &name,
        const boost::filesystem::path &config)
    {
        container_ptr ptr(
            ::lxc_container_new(name.c_str(), config.string().c_str()),
            false
        );
        if (!ptr)
            BOOST_THROW_EXCEPTION(
                UnableToCreateContainerError() <<
                UnableToCreateContainerError::name(name) <<
                UnableToCreateContainerError::config(config));
        return ptr;
    }
}}}}}
