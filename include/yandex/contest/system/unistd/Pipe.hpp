#pragma once

#include <yandex/contest/system/unistd/Descriptor.hpp>

#include <boost/noncopyable.hpp>

#include <system_error>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    class Pipe: boost::noncopyable
    {
    public:
        Pipe();

        int readEnd() const noexcept;
        int writeEnd() const noexcept;
        Descriptor releaseReadEnd() noexcept;
        Descriptor releaseWriteEnd() noexcept;
        void closeReadEnd(std::error_code &ec) noexcept;
        void closeReadEnd();
        void closeWriteEnd(std::error_code &ec) noexcept;
        void closeWriteEnd();
        bool readEndIsOpened() const noexcept;
        bool writeEndIsOpened() const noexcept;

    private:
        enum End: unsigned;

    private:
        Descriptor fd_[2];
    };
}}}}
