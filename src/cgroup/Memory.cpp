#include <yandex/contest/system/cgroup/Memory.hpp>

#include <yandex/contest/system/cgroup/Error.hpp>

namespace yandex {
namespace contest {
namespace system {
namespace cgroup {

const std::string MemoryBase::SUBSYSTEM_NAME("memory");
const boost::optional<std::string> MemoryBase::UNITS("bytes");

MemoryBase::MoveChargeAtImmigrateConfig::MoveChargeAtImmigrateConfig(
    const int mask)
    : anonymous(mask & (1 << 0)), file(mask & (1 << 1)) {}

int MemoryBase::MoveChargeAtImmigrateConfig::mask() const {
  return (static_cast<int>(anonymous) << 0) | (static_cast<int>(file) << 1);
}

MemoryBase::MoveChargeAtImmigrateConfig MemoryBase::moveChargeAtImmigrate()
    const {
  return MoveChargeAtImmigrateConfig(
      readField<int>("move_charge_at_immigrate"));
}

void MemoryBase::setMoveChargeAtImmigrate(
    const MoveChargeAtImmigrateConfig &moveChargeAtImmigrate) const {
  writeField("move_charge_at_immigrate", moveChargeAtImmigrate.mask());
}

void MemoryBase::setMoveChargeAtImmigrate(const bool anonymous,
                                          const bool file) const {
  MoveChargeAtImmigrateConfig cfg;
  cfg.anonymous = anonymous;
  cfg.file = file;
  setMoveChargeAtImmigrate(cfg);
}

Count MemoryBase::softLimitInBytes() const {
  return readField<Count>("soft_limit_in_bytes");
}

void MemoryBase::setSoftLimitInBytes(const Count limit) const {
  writeField("soft_limit_in_bytes", limit);
}

namespace {
struct KeyNotFoundInFileError : virtual Error {
  using key = boost::error_info<struct keyTag, std::string>;
  using file = boost::error_info<struct fileTag, std::string>;
};

template <typename T>
T parseKVFile(std::istream &in, const std::string &key) {
  std::string k;
  T v;
  while (in >> k >> v) {
    if (k == key) return v;
  }
  BOOST_THROW_EXCEPTION(KeyNotFoundInFileError()
                        << KeyNotFoundInFileError::key(key));
}
}  // namespace

bool MemoryBase::underOom() const {
  bool underOom_;
  try {
    readFieldByReader("oom_control", [&underOom_](std::istream &in) {
      underOom_ = parseKVFile<int>(in, "under_oom");
    });
  } catch (KeyNotFoundInFileError &e) {
    e << KeyNotFoundInFileError::file("oom_control");
    throw;
  }
  return underOom_;
}

bool MemoryBase::oomKillDisable() const {
  bool oomKillDisable_;
  try {
    readFieldByReader("oom_control", [&oomKillDisable_](std::istream &in) {
      oomKillDisable_ = parseKVFile<int>(in, "oom_kill_disable");
    });
  } catch (KeyNotFoundInFileError &e) {
    e << KeyNotFoundInFileError::file("oom_control");
    throw;
  }
  return oomKillDisable_;
}

void MemoryBase::setOomKillDisable(const bool oomControl) const {
  writeField<int>("oom_control", oomControl);
}

void MemoryBase::forceEmpty() const { writeField("force_empty", 0); }

}  // namespace cgroup
}  // namespace system
}  // namespace contest
}  // namespace yandex
