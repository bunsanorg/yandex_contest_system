#include <yandex/contest/system/cgroup/ProcPidCgroup.hpp>

#include <yandex/contest/system/cgroup/Error.hpp>

#include <bunsan/enable_error_info.hpp>
#include <bunsan/filesystem/fstream.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

namespace yandex{namespace contest{namespace system{namespace cgroup
{
    void ProcPidCgroup::load(const boost::filesystem::path &path)
    {
        entries.clear();
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            bunsan::filesystem::ifstream fin(path);
            BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin)
            {
                std::string line;
                while (std::getline(fin, line))
                {
                    entries.resize(entries.size() + 1);
                    Entry &entry = entries.back();
                    BUNSAN_EXCEPTIONS_WRAP_BEGIN()
                    {
                        const std::size_t pos1 = line.find(':');
                        if (pos1 == std::string::npos)
                            BOOST_THROW_EXCEPTION(
                                ProcPidCgroupNotEnoughFieldsError());
                        const std::size_t pos2 = line.find(':', pos1 + 1);
                        if (pos2 == std::string::npos)
                            BOOST_THROW_EXCEPTION(
                                ProcPidCgroupNotEnoughFieldsError());
                        BOOST_ASSERT(pos1 < pos2);
                        // Since last field may contain ':',
                        // we do not check exact number of ':'-separated fields.
                        entry.hierarchyId = boost::lexical_cast<std::size_t>(
                            line.substr(0, pos1)
                        );
                        const std::string subsystemsLine =
                            line.substr(pos1 + 1, pos2 - pos1 - 1);
                        boost::algorithm::split(
                            entry.subsystems,
                            subsystemsLine,
                            boost::algorithm::is_any_of(",")
                        );
                        entry.controlGroup = line.substr(pos2 + 1);
                    }
                    BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(
                        ProcPidCgroupLineFormatError::line(line))
                }
            }
            BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
            fin.close();
        }
        BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(FileFormatError::path(path))
    }
}}}}
