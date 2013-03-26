#include "yandex/contest/system/cgroup/ControlGroup.hpp"
#include "yandex/contest/system/cgroup/SystemInfo.hpp"

#include "yandex/contest/detail/IntrusivePointerHelper.hpp"

#include "bunsan/enable_error_info.hpp"
#include "bunsan/filesystem/fstream.hpp"

#include <boost/algorithm/string/trim.hpp>

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

    void ControlGroup::readFieldByReader(const std::string &fieldName, const Reader &reader)
    {
        const boost::filesystem::path fpath = fieldPath(fieldName);
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            bunsan::filesystem::ifstream fin(fpath);
            reader(fin);
            fin.close();
        }
        BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(ControlGroupError::path(fpath))
    }

    void ControlGroup::writeFieldByWriter(const std::string &fieldName, const Writer &writer)
    {
        const boost::filesystem::path fpath = fieldPath(fieldName);
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            bunsan::filesystem::ofstream fout(fpath);
            writer(fout);
            fout.close();
        }
        BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(ControlGroupError::path(fpath))
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
