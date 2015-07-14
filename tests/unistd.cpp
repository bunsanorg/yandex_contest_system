#define BOOST_TEST_MODULE unistd
#include <boost/test/unit_test.hpp>

#include <yandex/contest/system/unistd/MountEntry.hpp>

#include <boost/filesystem/operations.hpp>

namespace ya = yandex::contest;
namespace yas = ya::system;
namespace unistd = ya::system::unistd;

BOOST_AUTO_TEST_SUITE(MountEntry)

BOOST_AUTO_TEST_CASE(hasOpt) {
  BOOST_CHECK(unistd::MountEntry::bind("/hello", "/world").hasOpt("bind"));
  BOOST_CHECK(!unistd::MountEntry::bind("/hello", "/world").hasOpt("noopt"));
  BOOST_CHECK(unistd::MountEntry::bindRO("/hello", "/world").hasOpt("ro"));
}

BOOST_AUTO_TEST_CASE(escape) {
  BOOST_CHECK_EQUAL(unistd::MountEntry::escape("123"), "123");
  BOOST_CHECK_EQUAL(unistd::MountEntry::escape("string with spaces"),
                    "string\\040with\\040spaces");
}

BOOST_AUTO_TEST_CASE(unescape) {
  BOOST_CHECK_EQUAL(unistd::MountEntry::unescape("123"), "123");
  BOOST_CHECK_EQUAL(unistd::MountEntry::unescape("string\\040with\\040spaces"),
                    "string with spaces");
}

BOOST_AUTO_TEST_CASE(construction) {
  const unistd::MountEntry ent("/dev/sda3 /boot ext4 defaults 0 1");
  BOOST_CHECK_EQUAL(ent.fsname, "/dev/sda3");
  BOOST_CHECK_EQUAL(ent.dir, "/boot");
  BOOST_CHECK_EQUAL(ent.type, "ext4");
  BOOST_CHECK_EQUAL(ent.opts, "defaults");
  BOOST_CHECK_EQUAL(ent.freq, 0);
  BOOST_CHECK_EQUAL(ent.passno, 1);
}

BOOST_AUTO_TEST_CASE(construction_leading_trailing_spaces) {
  const unistd::MountEntry ent(" \t/dev/sda3 /boot ext4 defaults 0 1\n \n");
  BOOST_CHECK_EQUAL(ent.fsname, "/dev/sda3");
  BOOST_CHECK_EQUAL(ent.dir, "/boot");
  BOOST_CHECK_EQUAL(ent.type, "ext4");
  BOOST_CHECK_EQUAL(ent.opts, "defaults");
  BOOST_CHECK_EQUAL(ent.freq, 0);
  BOOST_CHECK_EQUAL(ent.passno, 1);
}

BOOST_AUTO_TEST_CASE(string) {
  unistd::MountEntry ent;
  ent.fsname = "/dev/sda1";
  ent.dir = "/";
  BOOST_CHECK_THROW(static_cast<std::string>(ent),
                    unistd::MountEntryFormatError);
  ent.type = "ext4";
  ent.opts = "defaults";
  BOOST_CHECK_EQUAL(static_cast<std::string>(ent),
                    "/dev/sda1 / ext4 defaults 0 0");
}

BOOST_AUTO_TEST_SUITE_END()

#include <yandex/contest/system/unistd/CharStarStar.hpp>

BOOST_AUTO_TEST_SUITE(CharStarStar)

BOOST_AUTO_TEST_CASE(general) {
  std::vector<std::string> arguments = {"some", "arbitrary", "string list"};
  unistd::CharStarStar argv(arguments,
                            unistd::CharStarStar::stringToVectorChar);
  char **argv_ = argv.get();
  BOOST_CHECK_EQUAL_COLLECTIONS(argv_, argv_ + arguments.size(),
                                arguments.begin(), arguments.end());
  BOOST_CHECK(argv_[arguments.size()] == nullptr);
}

BOOST_AUTO_TEST_CASE(empty) {
  std::vector<std::string> arguments = {};
  BOOST_REQUIRE_EQUAL(arguments.size(), 0);
  unistd::CharStarStar argv(arguments,
                            unistd::CharStarStar::stringToVectorChar);
  char **argv_ = argv.get();
  BOOST_CHECK(argv_[0] == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()

#include <yandex/contest/system/execution/CommonTypedefs.hpp>
#include <yandex/contest/system/unistd/Exec.hpp>

BOOST_AUTO_TEST_SUITE(Exec)

BOOST_AUTO_TEST_CASE(ExecEnvironmentEqKey) {
  const yas::execution::ProcessEnvironment environment = {
      {"incorrect=key", "value"}};
  BOOST_CHECK_THROW(unistd::Exec("", {}, environment),
                    unistd::InvalidEnvironmentKeyError);
}

struct ExecFixture {
  const boost::filesystem::path executable = "/some/path";
  const yas::execution::ProcessArguments fallbackArguments = {
      executable.string()};
  const yas::execution::ProcessArguments arguments = {"hello", "world"};
  const yas::execution::ProcessEnvironment environment = {
      {"PATH", "/bin:/usr/bin"}, {"SHELL", "/bin/sh"}, {"PWD", "/home"}};
};

BOOST_FIXTURE_TEST_CASE(ExecExecutable, ExecFixture) {
  unistd::Exec exec(executable);
  BOOST_CHECK_EQUAL(exec.executable(), executable.c_str());
  BOOST_CHECK_EQUAL_COLLECTIONS(
      exec.argv(), exec.argv() + fallbackArguments.size(),
      fallbackArguments.begin(), fallbackArguments.end());
  BOOST_CHECK(exec.argv()[fallbackArguments.size()] == nullptr);
  BOOST_CHECK(exec.envp()[0] == nullptr);
}

BOOST_FIXTURE_TEST_CASE(ExecExecutableArguments, ExecFixture) {
  unistd::Exec exec(executable, arguments);
  BOOST_CHECK_EQUAL(exec.executable(), executable.c_str());
  BOOST_CHECK_EQUAL_COLLECTIONS(exec.argv(), exec.argv() + arguments.size(),
                                arguments.begin(), arguments.end());
  BOOST_CHECK(exec.argv()[arguments.size()] == nullptr);
  BOOST_CHECK(exec.envp()[0] == nullptr);
}

BOOST_FIXTURE_TEST_CASE(ExecExecutableArgumentsEnvironment, ExecFixture) {
  unistd::Exec exec(executable, arguments, environment);
  BOOST_CHECK_EQUAL(exec.executable(), executable.c_str());
  BOOST_CHECK_EQUAL_COLLECTIONS(exec.argv(), exec.argv() + arguments.size(),
                                arguments.begin(), arguments.end());
  BOOST_CHECK(exec.argv()[arguments.size()] == nullptr);
  const std::vector<std::string> envp(exec.envp(),
                                      exec.envp() + environment.size());
  yas::execution::ProcessEnvironment environ;
  for (const std::string &s : envp) {
    const std::size_t eq = s.find('=');
    environ[s.substr(0, eq)] = s.substr(eq + 1);
  }
  BOOST_CHECK(environ == environment);
}

BOOST_AUTO_TEST_SUITE_END()

#include <yandex/contest/system/unistd/Operations.hpp>

BOOST_AUTO_TEST_SUITE(Operations)

struct CreateDirectoryFixture {
  CreateDirectoryFixture()
      : path(boost::filesystem::temp_directory_path() /
             boost::filesystem::unique_path()) {}

  ~CreateDirectoryFixture() { boost::filesystem::remove(path); }

  const boost::filesystem::path path;
};

BOOST_FIXTURE_TEST_CASE(create_directory, CreateDirectoryFixture) {
  BOOST_CHECK(unistd::create_directory(path, 0777));
  BOOST_CHECK(!unistd::create_directory(path, 0777));
  BOOST_CHECK(boost::filesystem::remove(path));
}

BOOST_AUTO_TEST_SUITE_END()  // Operations

#include <yandex/contest/system/unistd/ProcessResult.hpp>

BOOST_AUTO_TEST_SUITE(ProcessResult)

BOOST_AUTO_TEST_CASE(operator_bool) {
  unistd::ProcessResult result;
  result.exitStatus = 0;
  result.termSig.reset();
  BOOST_CHECK(result);
  result.exitStatus = 1;
  BOOST_CHECK(!result);
  result.termSig = 1;
  BOOST_CHECK(!result);
  result.exitStatus.reset();
  BOOST_CHECK(!result);
  result.termSig.reset();
  BOOST_CHECK(!result);
  result.exitStatus = 0;
  BOOST_CHECK(result);
}

BOOST_AUTO_TEST_SUITE_END()

#include <yandex/contest/system/unistd/Descriptor.hpp>
#include <yandex/contest/system/unistd/Operations.hpp>
#include <yandex/contest/system/unistd/Pipe.hpp>

BOOST_AUTO_TEST_SUITE(descriptor)

BOOST_AUTO_TEST_CASE(Descriptor) {
  unistd::Descriptor fd1 = unistd::dup(1);
  BOOST_REQUIRE(fd1);
  const int fd = fd1.get();
  unistd::Descriptor fd2(fd1.release());
  BOOST_REQUIRE(!fd1);
  BOOST_REQUIRE_EQUAL(fd2.get(), fd);
}

BOOST_AUTO_TEST_CASE(Pipe) {
  unistd::Pipe pipe;
  BOOST_REQUIRE(pipe.readEndIsOpened());
  BOOST_REQUIRE(pipe.writeEndIsOpened());
  const int rfd = pipe.readEnd(), wfd = pipe.writeEnd();

  unistd::Descriptor readEnd = pipe.releaseReadEnd();
  BOOST_REQUIRE(!pipe.readEndIsOpened());
  BOOST_REQUIRE(pipe.writeEndIsOpened());
  BOOST_REQUIRE(readEnd);
  BOOST_REQUIRE_EQUAL(readEnd.get(), rfd);

  unistd::Descriptor writeEnd(pipe.releaseWriteEnd().release());
  BOOST_REQUIRE(!pipe.readEndIsOpened());
  BOOST_REQUIRE(!pipe.writeEndIsOpened());
  BOOST_REQUIRE(writeEnd);
  BOOST_REQUIRE_EQUAL(writeEnd.get(), wfd);
}

BOOST_AUTO_TEST_SUITE_END()  // descriptor
