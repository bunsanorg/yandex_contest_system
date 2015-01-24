#include <yandex/contest/system/unistd/SysConfBuffer.hpp>

#include <unistd.h>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    static const std::size_t BUFFER_SIZE = std::size_t(1) << 14;
    static const long MAX_SYS_BUFFER_SIZE = long(1) << 20;

    SysConfBuffer::SysConfBuffer(const int name)
    {
        reset(name);
    }

    void SysConfBuffer::expand()
    {
        if (m_buffer.empty())
            m_buffer.resize(BUFFER_SIZE);
        else
            m_buffer.resize(m_buffer.size() * 2);
    }

    void SysConfBuffer::reset()
    {
        m_buffer.clear();
        m_buffer.resize(BUFFER_SIZE);
    }

    void SysConfBuffer::reset(const int name)
    {
        const long buffer_size = sysconf(name);
        if (0 < buffer_size && buffer_size < MAX_SYS_BUFFER_SIZE)
        {
            m_buffer.clear();
            m_buffer.resize(buffer_size);
        }
        else
        {
            reset();
        }
    }
}}}}
