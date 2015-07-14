#include <yandex/contest/system/unistd/access/Id.hpp>

namespace yandex {
namespace contest {
namespace system {
namespace unistd {
namespace access {

bool Id::operator==(const Id &id) const noexcept {
  return (uid == id.uid) && (gid == id.gid);
}

std::ostream &operator<<(std::ostream &out, const Id &id) {
  return out << "{uid = " << id.uid << ", gid = " << id.gid << "}";
}

}  // namespace access
}  // namespace unistd
}  // namespace system
}  // namespace contest
}  // namespace yandex
