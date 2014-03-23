#pragma once

#include <system_error>

#include <boost/optional.hpp>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    class Descriptor
    {
    public:
        Descriptor()=default;
        explicit Descriptor(const int fd) noexcept;
        Descriptor(Descriptor &&) noexcept;
        Descriptor &operator=(Descriptor &&) noexcept;

        Descriptor(const Descriptor &)=delete;
        Descriptor &operator=(const Descriptor &)=delete;

        ~Descriptor();

        void assign(const int fd) noexcept;

        int get() const noexcept;

        void close();
        void close(std::error_code &ec) noexcept;

        int release() noexcept;

        explicit operator bool() const noexcept;

        void swap(Descriptor &descriptor) noexcept;

    private:
        void closeNoExcept() noexcept;

    private:
        boost::optional<int> fd_;
    };

    inline void swap(Descriptor &a, Descriptor &b) noexcept
    {
        a.swap(b);
    }
}}}}
