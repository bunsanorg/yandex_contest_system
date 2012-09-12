#pragma once

#include "yandex/contest/Error.hpp"

#include <memory>

#include <boost/filesystem/path.hpp>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    struct DynamicLibraryError: virtual Error
    {
        typedef boost::error_info<struct filenameTag, boost::filesystem::path> filename;
        typedef boost::error_info<struct flagsTag, int> flags;
        typedef boost::error_info<struct dlerrorTag, std::string> dlerror;
    };

    struct DynamicLibraryIsAlreadyResidentError: virtual DynamicLibraryError {};

    namespace detail
    {
        struct DynamicLibraryDelete
        {
            void operator()(void *ptr) const noexcept;
        };
    };

    /// \warning Does not support multiple load of one library.
    class DynamicLibrary
    {
    public:
        DynamicLibrary();

        DynamicLibrary(const boost::filesystem::path &filename, const int flags);

        DynamicLibrary(DynamicLibrary &&dl);
        DynamicLibrary &operator=(DynamicLibrary &&dl);

        DynamicLibrary(const DynamicLibrary &)=delete;
        DynamicLibrary &operator=(const DynamicLibrary &)=delete;

        void swap(DynamicLibrary &dl) noexcept;

        explicit operator bool() const noexcept;

        void close();

        ~DynamicLibrary();

    private:
        void *handle_;
    };

    inline void swap(DynamicLibrary &a, DynamicLibrary &b) noexcept
    {
        a.swap(b);
    }
}}}}
