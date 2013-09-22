#include <yandex/contest/system/cgroup/ControlGroup.hpp>

#include <yandex/contest/system/cgroup/SystemInfo.hpp>

#include <yandex/contest/detail/IntrusivePointerHelper.hpp>

#include <bunsan/enable_error_info.hpp>
#include <bunsan/filesystem/fstream.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/operations.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    YANDEX_CONTEST_INTRUSIVE_PTR_DEFINE(ControlGroup)

    ControlGroupPointer ControlGroup::attachChild(const boost::filesystem::path &childControlGroup)
    {
        return attachChild__(childControlGroup);
    }

    ControlGroupPointer ControlGroup::createChild(const boost::filesystem::path &childControlGroup)
    {
        return createChild(childControlGroup, 0777);
    }

    ControlGroupPointer ControlGroup::createChild(const boost::filesystem::path &childControlGroup, const mode_t mode)
    {
        return createChild__(childControlGroup, mode);
    }

    ControlGroupPointer ControlGroup::parent()
    {
        return parent__();
    }

    boost::filesystem::path ControlGroup::fieldPath(const std::string &fieldName) const
    {
        const boost::filesystem::path name(fieldName);
        if (name.is_absolute() || name.filename() != name)
            BOOST_THROW_EXCEPTION(ControlGroupInvalidFieldNameError() <<
                                  ControlGroupInvalidFieldNameError::fieldName(fieldName));
        const boost::filesystem::path path = fieldPath__(fieldName);
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            const boost::filesystem::file_status status = boost::filesystem::symlink_status(path);
            switch (status.type())
            {
            case boost::filesystem::regular_file:
                // OK
                break;
            case boost::filesystem::file_not_found:
                BOOST_THROW_EXCEPTION(ControlGroupFieldDoesNotExistError());
            default:
                BOOST_THROW_EXCEPTION(ControlGroupInvalidFieldFileError());
            }
        }
        BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(
            ControlGroupFieldError::fieldName(fieldName) <<
            ControlGroupFieldError::fieldPath(path))
        return path;
    }

    void ControlGroup::readFieldByReader(const std::string &fieldName, const Reader &reader)
    {
        const boost::filesystem::path fpath = fieldPath(fieldName);
        bunsan::filesystem::ifstream fin(fpath);
        BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin)
        {
            reader(fin);
        }
        BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
        fin.close();
    }

    void ControlGroup::writeFieldByWriter(const std::string &fieldName, const Writer &writer)
    {
        const boost::filesystem::path fpath = fieldPath(fieldName);
        bunsan::filesystem::ofstream fout(fpath);
        BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout)
        {
            writer(fout);
        }
        BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
        fout.close();
    }

    template <>
    void ControlGroup::readField(const std::string &fieldName, const detail::IStreamableWrapper &data)
    {
        readFieldByReader(fieldName,
            [&data](std::istream &in)
            {
                in >> data;
            });
    }

    template <>
    void ControlGroup::writeField(const std::string &fieldName, const detail::OStreamableWrapper &data)
    {
        writeFieldByWriter(fieldName,
            [&data](std::ostream &out)
            {
                out << data;
            });
    }

    template <>
    std::string ControlGroup::readFieldAll(const std::string &fieldName)
    {
        std::string data;
        readFieldByReader(fieldName,
            [&data](std::istream &in)
            {
                data.assign(std::istreambuf_iterator<char>(in),
                            std::istreambuf_iterator<char>());
            });
        return data;
    }

    template <>
    std::string ControlGroup::readFieldAllRtrimmed(const std::string &fieldName)
    {
        std::string data = readFieldAll<std::string>(fieldName);
        boost::algorithm::trim_right(data);
        return data;
    }

    std::ostream &operator<<(std::ostream &out, const ControlGroup &cgroup)
    {
        cgroup.print(out);
        return out;
    }
}}}}
