#include <yandex/contest/system/unistd/DynamicLoader.hpp>

#include <yandex/contest/detail/LogHelper.hpp>
#include <yandex/contest/TypeInfo.hpp>

#include <boost/format.hpp>

#include <dlfcn.h>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    DynamicLibrary::DynamicLibrary():
        handle_(nullptr) {}

    DynamicLibrary::DynamicLibrary(const boost::filesystem::path &filename, const int flags):
        handle_(dlopen(filename.c_str(), flags | RTLD_NOLOAD))
    {
        const boost::filesystem::path fname = (filename.filename() == filename) ?
            str(boost::format("lib%1%.so") % filename.string()) : filename;
        if (handle_)
            BOOST_THROW_EXCEPTION(DynamicLibraryIsAlreadyResidentError() <<
                                  DynamicLibraryError::filename(filename) <<
                                  DynamicLibraryError::flags(flags));
        handle_ = dlopen(fname.c_str(), flags);
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

    DynamicLibrary::DynamicLibrary(DynamicLibrary &&dl) noexcept
    {
        swap(dl);
        dl.closeNoExcept();
    }

    DynamicLibrary &DynamicLibrary::operator=(DynamicLibrary &&dl) noexcept
    {
        swap(dl);
        dl.closeNoExcept();
        return *this;
    }

    void DynamicLibrary::closeNoExcept() noexcept
    {
        try
        {
            close();
        }
        catch (std::exception &e)
        {
            STREAM_ERROR << "Error while closing DynamicLibrary (ignoring): " << e.what();
        }
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

    namespace
    {
        template <typename T>
        std::ostream &print(std::ostream &out, const boost::optional<T> &opt)
        {
            if (opt)
                out << opt.get();
            else
                out << "???";
            return out;
        }
    }

    std::ostream &operator<<(std::ostream &out, const DLInfo &info)
    {
        out << info.fname.string() << "(";
        print(out, info.sname);
        if (info.offset)
        {
            out << "+0x" << std::hex << info.offset;
        }
        out << ") [";
        print(out, info.saddr);
        out << "]";
        return out;
    }

    namespace detail
    {
        DLInfo dladdr(void *const addr)
        {
            Dl_info info;
            if (!dladdr(addr, &info))
            {
                const char *const err = dlerror();
                BOOST_ASSERT(err);
                BOOST_THROW_EXCEPTION(DynamicLoaderError() << DynamicLoaderError::dlerror(err));
            }
            DLInfo ret;
            ret.fname = info.dli_fname;
            ret.fbase = info.dli_fbase;
            if (info.dli_sname)
                ret.sname = typeinfo::demangle(info.dli_sname);
            if (info.dli_saddr)
            {
                ret.saddr = info.dli_saddr;
                ret.offset = static_cast<char *>(addr) - static_cast<char *>(info.dli_saddr);
            }
            return ret;
        }
    }
}}}}
