#pragma once

#include <yandex/contest/system/unistd/CharStarStar.hpp>
#include <yandex/contest/system/unistd/Error.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>

#include <string>
#include <system_error>
#include <unordered_map>
#include <vector>

namespace yandex {
namespace contest {
namespace system {
namespace unistd {

struct InvalidEnvironmentKeyError : virtual Error {
  using key = boost::error_info<struct keyTag, std::string>;
};

namespace info {
using executable =
    boost::error_info<struct executableTag, boost::filesystem::path>;
}

class Exec : private boost::noncopyable {
 public:
  using ProcessArguments = std::vector<std::string>;
  using ProcessEnvironment = std::unordered_map<std::string, std::string>;

 public:
  explicit Exec(const boost::filesystem::path &executable,
                const ProcessArguments &arguments = ProcessArguments(),
                const ProcessEnvironment &environment = ProcessEnvironment());

  const char *executable() const;
  char **argv();
  char **envp();

  /// System call, see exec(3).
  void execv(std::error_code &ec) const noexcept;

  /// \copydoc execv()
  void execv() const;

  /// \copydoc execv()
  void execvp(std::error_code &ec) const noexcept;

  /// \copydoc execv()
  void execvp() const;

  /// \copydoc execv()
  void execve(std::error_code &ec) const noexcept;

  /// \copydoc execv()
  void execve() const;

  /// \copydoc execv()
  void execvpe(std::error_code &ec) const noexcept;

  /// \copydoc execv()
  void execvpe() const;

 private:
  /*!
   * According to environ(3),
   * "The statement about argv[] and envp[] being constants is included
   * to make explicit to future writers of language bindings that these
   * objects are completely constant."
   * So, it is safe to use it in system calls with constant Exec object.
   * Nevertheless, it is not safe to export it to public interface
   * without constant modifier.
   *
   * \warning Do not modify returned data.
   */
  char **argv_() const;

  /// \copydoc argv_()
  char **envp_() const;

 private:
  boost::filesystem::path executable_;
  CharStarStar argvWrapper_, envpWrapper_;
};

}  // namespace unistd
}  // namespace system
}  // namespace contest
}  // namespace yandex
