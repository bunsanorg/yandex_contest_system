#include "yandex/contest/system/unistd/DynamicLibrary.hpp"

#include <dlfcn.h>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    DynamicLibrary::DynamicLibrary():
        handle_(nullptr) {}

    DynamicLibrary::DynamicLibrary(const boost::filesystem::path &filename, const int flags):
        handle_(dlopen(filename.c_str(), flags | RTLD_NOLOAD))
    {
        if (handle_)
            BOOST_THROW_EXCEPTION(DynamicLibraryIsAlreadyResidentError() <<
                                  DynamicLibraryError::filename(filename) <<
                                  DynamicLibraryError::flags(flags));
        handle_ = dlopen(filename.c_str(), flags);
        if (!handle_)
        {
            const char *const err = dlerror();
            BOOST_ASSERT(err);
            BOOST_THROW_EXCEPTION(DynamicLibraryError() <<
                                  DynamicLibraryError::filename(filename) <<
                                  DynamicLibraryError::dlerror(err) <<
                                  DynamicLibraryError::flags(flags));
        }
    }

    DynamicLibrary::DynamicLibrary(DynamicLibrary &&dl)
    {
        swap(dl);
        dl.close();
    }

    DynamicLibrary &DynamicLibrary::operator=(DynamicLibrary &&dl)
    {
        swap(dl);
        dl.close();
        return *this;
    }

    void DynamicLibrary::close()
    {
        if (handle_)
        {
            if (dlclose(handle_))
            {
                const char *const err = dlerror();
                BOOST_ASSERT(err);
                BOOST_THROW_EXCEPTION(DynamicLibraryError() << DynamicLibraryError::dlerror(err));
            }
        }
    }

    void DynamicLibrary::swap(DynamicLibrary &dl) noexcept
    {
        using std::swap;
        swap(handle_, dl.handle_);
    }

    DynamicLibrary::operator bool() const noexcept
    {
        return handle_;
    }

    void *DynamicLibrary::symbol(const char *const sym)
    {
        return dlsym(handle_, sym);
    }

    void *DynamicLibrary::symbol(const std::string &sym)
    {
        return symbol(sym.c_str());
    }

    DynamicLibrary::~DynamicLibrary()
    {
        try
        {
            close();
        }
        catch (...)
        {
            // does nothing
        }
    }
}}}}
