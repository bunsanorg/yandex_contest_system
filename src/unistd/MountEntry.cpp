#include "yandex/contest/system/unistd/MountEntry.hpp"

#include <sstream>
#include <iomanip>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    MountEntry::MountEntry(const std::string &line_)
    {
        const std::string line = boost::algorithm::trim_copy(line_);
        std::vector<std::string> splitLine;
        boost::algorithm::split(splitLine, line, boost::is_any_of(" \t"), boost::token_compress_on);
        if (splitLine.size() != 6)
            BOOST_THROW_EXCEPTION(MountEntryInvalidRepresentationError() <<
                                  Error::message("Wrong fields number") <<
                                  MountEntryInvalidRepresentationError::line(line));
        fsname = unescape(splitLine[0]);
        dir = unescape(splitLine[1]);
        type = unescape(splitLine[2]);
        opts = unescape(splitLine[3]);
        // unescape is useless here
        freq = boost::lexical_cast<int>(splitLine[4]);
        passno = boost::lexical_cast<int>(splitLine[5]);
    }

#define MOUNT_ENTRY_ASSIGN_NON_EMPTY(INDEX, FIELD) \
    if (FIELD.empty()) \
        BOOST_THROW_EXCEPTION(MountEntryUninitializedFieldError() << \
                              MountEntryUninitializedFieldError::field(#FIELD)); \
    else \
        splitLine[INDEX] = escape(FIELD)

    MountEntry::operator std::string() const
    {
        std::vector<std::string> splitLine(6);
        MOUNT_ENTRY_ASSIGN_NON_EMPTY(0, fsname);
        MOUNT_ENTRY_ASSIGN_NON_EMPTY(1, dir);
        MOUNT_ENTRY_ASSIGN_NON_EMPTY(2, type);
        MOUNT_ENTRY_ASSIGN_NON_EMPTY(3, opts);
        splitLine[4] = boost::lexical_cast<std::string>(freq);
        splitLine[5] = boost::lexical_cast<std::string>(passno);
        return boost::join(splitLine, " ");
    }

    std::string MountEntry::escape(const std::string &field)
    {
        std::ostringstream buf;
        for (const char c: field)
        {
            switch (c)
            {
            case ' ':
            case '\t':
            case '\n':
            case '\\':
                buf << '\\';
                buf << std::setw(3) << std::setfill('0');
                buf << std::oct << static_cast<unsigned>(c);
                break;
            default:
                buf << c;
            }
        }
        return buf.str();
    }

    std::string MountEntry::unescape(const std::string &field)
    {
        std::string buf;
        unsigned octal = 0;
        unsigned char code;
        for (const char c: field)
        {
            if (octal)
            {
                code *= 8;
                code += (c - '0');
                ++octal;
                if (octal == 4)
                {
                    buf.push_back(static_cast<char>(code));
                    octal = 0;
                }
            }
            else
            {
                if (c == '\\')
                {
                    octal = 1;
                    code = 0;
                }
                else
                {
                    buf.push_back(c);
                }
            }
        }
        return buf;
    }

    std::ostream &operator<<(std::ostream &out, const MountEntry &entry)
    {
        return out << static_cast<std::string>(entry);
    }
}}}}
