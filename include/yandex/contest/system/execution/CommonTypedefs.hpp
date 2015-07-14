#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace yandex {
namespace contest {
namespace system {
namespace execution {

using ProcessArguments = std::vector<std::string>;
using ProcessEnvironment = std::unordered_map<std::string, std::string>;

}  // namespace execution
}  // namespace system
}  // namespace contest
}  // namespace yandex
