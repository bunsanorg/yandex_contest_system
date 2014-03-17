#pragma once

#include <algorithm>
#include <vector>
#include <string>

#include <boost/noncopyable.hpp>

namespace yandex{namespace contest{namespace system{namespace unistd
{
    /*!
     * \brief Converts container of strings to char **.
     *
     * Can be useful for C-API.
     */
    class CharStarStar: private boost::noncopyable
    {
    public:
        template <typename Container, typename UnaryOperatpr>
        CharStarStar(const Container &container, const UnaryOperatpr &op):
            vvc_(container.size()),
            vcs_(container.size() + 1)
        {
            std::transform(
                container.begin(), container.end(), vvc_.begin(), op
            );
            vcs_.resize(vvc_.size() + 1, nullptr);
            const auto vchar2charStar =
                [](std::vector<char> &v)
                {
                    return &v[0];
                };
            std::transform(
                vvc_.begin(), vvc_.end(),
                vcs_.begin(), vchar2charStar
            );
            vcs_[vvc_.size()] = nullptr;
        }

        char **get();

    public:
        static std::vector<char> stringToVectorChar(const std::string &s);

    private:
        std::vector<std::vector<char>> vvc_;
        std::vector<char *> vcs_;
    };
}}}}
