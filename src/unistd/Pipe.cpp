#include "yandex/contest/system/unistd/Pipe.hpp"

#include "yandex/contest/SystemError.hpp"

#include <boost/assert.hpp>

#include <unistd.h>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    enum Pipe::End: unsigned
    {
        READ_END,
        WRITE_END
    };

    Pipe::Pipe()
    {
        int fildes_[2];
        if (::pipe(fildes_) < 0)
            BOOST_THROW_EXCEPTION(SystemError("pipe"));
        for (int i = 0; i < 2; ++i)
            fd_[i].assign(fildes_[i]);
    }

    int Pipe::readEnd() const noexcept
    {
        BOOST_ASSERT(fd_[READ_END]);
        return fd_[READ_END].get();
    }

    int Pipe::writeEnd() const noexcept
    {
        BOOST_ASSERT(fd_[WRITE_END]);
        return fd_[WRITE_END].get();
    }

    Descriptor Pipe::releaseReadEnd() noexcept
    {
        return std::move(fd_[READ_END]);
    }

    Descriptor Pipe::releaseWriteEnd() noexcept
    {
        return std::move(fd_[WRITE_END]);
    }

    void Pipe::closeReadEnd(std::error_code &ec) noexcept
    {
        fd_[READ_END].close(ec);
    }

    void Pipe::closeReadEnd()
    {
        fd_[READ_END].close();
    }

    void Pipe::closeWriteEnd(std::error_code &ec) noexcept
    {
        fd_[WRITE_END].close(ec);
    }

    void Pipe::closeWriteEnd()
    {
        fd_[WRITE_END].close();
    }

    bool Pipe::readEndIsOpened() const noexcept
    {
        return static_cast<bool>(fd_[READ_END]);
    }

    bool Pipe::writeEndIsOpened() const noexcept
    {
        return static_cast<bool>(fd_[WRITE_END]);
    }
}}}}
