#include <yandex/contest/system/unistd/Exec.hpp>

#include <boost/assert.hpp>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    namespace
    {
        std::vector<char> stringPairToVectorChar(
            const std::pair<std::string, std::string> &p)
        {
            if (p.first.find('=') != std::string::npos)
                BOOST_THROW_EXCEPTION(
                    InvalidEnvironmentKeyError() <<
                    InvalidEnvironmentKeyError::key(p.first) <<
                    Error::message("Environment key may not contain '='."));
            std::vector<char> v(p.first.size() + 1 + p.second.size() + 1);
            auto dst = std::copy(p.first.begin(), p.first.end(), v.begin());
            *(dst++) = '=';
            dst = std::copy(p.second.begin(), p.second.end(), dst);
            *(dst++) = '\0';
            BOOST_ASSERT(dst == v.end());
            return v;
        }
    }

    Exec::Exec(const boost::filesystem::path &executable,
               const ProcessArguments &arguments,
               const ProcessEnvironment &environment):
        executable_(executable),
        argvWrapper_(
            arguments.empty() ?
                ProcessArguments{executable.string()} :
                arguments,
            CharStarStar::stringToVectorChar),
        envpWrapper_(environment, stringPairToVectorChar)
    {
    }

    const char *Exec::executable() const
    {
        return executable_.c_str();
    }

    char **Exec::argv()
    {
        return argvWrapper_.get();
    }

    char **Exec::envp()
    {
        return envpWrapper_.get();
    }

    char **Exec::argv_() const
    {
        return const_cast<CharStarStar &>(argvWrapper_).get();
    }

    char **Exec::envp_() const
    {
        return const_cast<CharStarStar &>(envpWrapper_).get();
    }
}}}}
