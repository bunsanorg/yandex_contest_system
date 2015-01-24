#pragma once

#include <vector>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    class SysConfBuffer
    {
    public:
        explicit SysConfBuffer(const int name);

        std::size_t size() const { return m_buffer.size(); }

        char *data() { return &m_buffer[0]; }
        const char *data() const { return &m_buffer[0]; }

        void expand();

        void reset();
        void reset(const int name);

    private:
        std::vector<char> m_buffer;
    };
}}}}
