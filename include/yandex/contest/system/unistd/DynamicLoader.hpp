#pragma once

#include <yandex/contest/Error.hpp>

#include <memory>

#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    struct DynamicLoaderError: virtual Error
    {
        typedef boost::error_info<struct dlerrorTag, std::string> dlerror;
    };

    struct DynamicLibraryError: virtual DynamicLoaderError
    {
        typedef boost::error_info<struct filenameTag, boost::filesystem::path> filename;
        typedef boost::error_info<struct flagsTag, int> flags;
    };

    struct DynamicLibraryIsAlreadyResidentError: virtual DynamicLoaderError {};

    /// \warning Does not support multiple load of one library.
    class DynamicLibrary
    {
    public:
        DynamicLibrary();

        /*!
         * If filename does not contain '/', it is transformed into lib${filename}.so.
         * That means that user may specify library similar to compiler option:
         * "-lm" or "-l/usr/lib/libm.so" <=>
         * DynamicLibrary("m", ...) or DynamicLibrary("/usr/lib/libm.so", ...).
         */
        DynamicLibrary(const boost::filesystem::path &filename, const int flags);

        DynamicLibrary(DynamicLibrary &&dl) noexcept;
        DynamicLibrary &operator=(DynamicLibrary &&dl) noexcept;

        DynamicLibrary(const DynamicLibrary &)=delete;
        DynamicLibrary &operator=(const DynamicLibrary &)=delete;

        void swap(DynamicLibrary &dl) noexcept;

        explicit operator bool() const noexcept;

        void close();

        void *symbol(const char *const sym);

        void *symbol(const std::string &sym);

        template <typename T, typename Sym>
        T symbol(const Sym &sym)
        {
            return reinterpret_cast<T>(symbol(sym));
        }

        ~DynamicLibrary();

    private:
        void closeNoExcept() noexcept;

    private:
        void *handle_;
    };

    inline void swap(DynamicLibrary &a, DynamicLibrary &b) noexcept
    {
        a.swap(b);
    }

    struct DLInfo
    {
        boost::filesystem::path fname;
        void *fbase = nullptr;
        boost::optional<std::string> sname;
        boost::optional<void *> saddr;
        boost::optional<std::size_t> offset;
    };

    std::ostream &operator<<(std::ostream &out, const DLInfo &info);

    namespace detail
    {
        DLInfo dladdr(void *const addr);
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
    template <typename Ret, typename ... Args>
    DLInfo dladdr(Ret (*fn)(Args...))
    {
        return detail::dladdr(reinterpret_cast<void *>(fn));
    }

    template <typename Class, typename Ret, typename ... Args>
    DLInfo dladdr(Ret (Class::*memfn)(Args...))
    {
        return detail::dladdr(reinterpret_cast<void *>(memfn));
    }
#pragma GCC diagnostic pop
}}}}
