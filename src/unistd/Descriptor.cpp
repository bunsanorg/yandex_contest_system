#include <yandex/contest/system/unistd/Descriptor.hpp>

#include <yandex/contest/SystemError.hpp>

#include <boost/assert.hpp>

#include <errno.h>

#include <unistd.h>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    Descriptor::Descriptor(const int fd) noexcept: fd_(fd) {}

    Descriptor::Descriptor(Descriptor &&descriptor) noexcept
    {
        swap(descriptor);
        descriptor.closeNoExcept();
    }

    Descriptor &Descriptor::operator=(Descriptor &&descriptor) noexcept
    {
        swap(descriptor);
        descriptor.closeNoExcept();
        return *this;
    }

    Descriptor::~Descriptor()
    {
        closeNoExcept();
    }

    void Descriptor::assign(const int fd) noexcept
    {
        closeNoExcept();
        fd_ = fd;
    }

    int Descriptor::get() const noexcept
    {
        BOOST_ASSERT(*this);
        return fd_.get();
    }

    void Descriptor::close()
    {
        std::error_code ec;
        close(ec);
        if (ec)
            BOOST_THROW_EXCEPTION(SystemError(ec, __func__));
    }

    void Descriptor::close(std::error_code &ec) noexcept
    {
        ec.clear();
        if (fd_)
        {
            const int ret = ::close(fd_.get());
            if (ret < 0)
                ec.assign(errno, std::system_category());
            fd_.reset();
        }
    }

    void Descriptor::closeNoExcept() noexcept
    {
        std::error_code ec;
        close(ec);
        // ignore ec
    }

    Descriptor::operator bool() const noexcept
    {
        return fd_;
    }

    void Descriptor::swap(Descriptor &descriptor) noexcept
    {
        using boost::swap;
        swap(fd_, descriptor.fd_);
    }
}}}}
