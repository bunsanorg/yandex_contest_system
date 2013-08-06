#include <yandex/contest/system/unistd/CharStarStar.hpp>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    char **CharStarStar::get()
    {
        return &vcs_[0];
    }

    std::vector<char> CharStarStar::stringToVectorChar(const std::string &s)
    {
        std::vector<char> v(s.size() + 1, '\0');
        std::copy(s.begin(), s.end(), v.begin());
        return v;
    }
}}}}
