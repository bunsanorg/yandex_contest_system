#define BOOST_TEST_MODULE unistd
#include <boost/test/unit_test.hpp>

#include "yandex/contest/system/unistd/MountEntry.hpp"

namespace ya = yandex::contest::system::unistd;

BOOST_AUTO_TEST_SUITE(MountEntry)

BOOST_AUTO_TEST_CASE(escape)
{
    BOOST_CHECK_EQUAL(ya::MountEntry::escape("123"), "123");
    BOOST_CHECK_EQUAL(ya::MountEntry::escape("string with spaces"), "string\\040with\\040spaces");
}

BOOST_AUTO_TEST_CASE(unescape)
{
    BOOST_CHECK_EQUAL(ya::MountEntry::unescape("123"), "123");
    BOOST_CHECK_EQUAL(ya::MountEntry::unescape("string\\040with\\040spaces"), "string with spaces");
}

BOOST_AUTO_TEST_CASE(construction)
{
    const ya::MountEntry ent("/dev/sda3 /boot ext4 defaults 0 1");
    BOOST_CHECK_EQUAL(ent.fsname, "/dev/sda3");
    BOOST_CHECK_EQUAL(ent.dir, "/boot");
    BOOST_CHECK_EQUAL(ent.type, "ext4");
    BOOST_CHECK_EQUAL(ent.opts, "defaults");
    BOOST_CHECK_EQUAL(ent.freq, 0);
    BOOST_CHECK_EQUAL(ent.passno, 1);
}

BOOST_AUTO_TEST_CASE(string)
{
    ya::MountEntry ent;
    ent.fsname = "/dev/sda1";
    ent.dir = "/";
    BOOST_CHECK_THROW(static_cast<std::string>(ent), ya::MountEntryFormatError);
    ent.type = "ext4";
    ent.opts = "defaults";
    BOOST_CHECK_EQUAL(static_cast<std::string>(ent), "/dev/sda1 / ext4 defaults 0 0");
}

BOOST_AUTO_TEST_SUITE_END()

#include "yandex/contest/system/unistd/CharStarStar.hpp"

BOOST_AUTO_TEST_SUITE(CharStarStar)

BOOST_AUTO_TEST_CASE(general)
{
    std::vector<std::string> arguments = {"some", "arbitrary", "string list"};
    ya::CharStarStar argv(arguments, ya::CharStarStar::stringToVectorChar);
    char **argv_ = argv.get();
    BOOST_CHECK_EQUAL_COLLECTIONS(argv_, argv_+arguments.size(), arguments.begin(), arguments.end());
    BOOST_CHECK(argv_[arguments.size()] == nullptr);
}

BOOST_AUTO_TEST_CASE(empty)
{
    std::vector<std::string> arguments = {};
    BOOST_REQUIRE_EQUAL(arguments.size(), 0);
    ya::CharStarStar argv(arguments, ya::CharStarStar::stringToVectorChar);
    char **argv_ = argv.get();
    BOOST_CHECK(argv_[0] == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()

#include "yandex/contest/system/execution/CommonTypedefs.hpp"
#include "yandex/contest/system/unistd/Exec.hpp"

BOOST_AUTO_TEST_SUITE(Exec)

BOOST_AUTO_TEST_CASE(ExecEnvironmentEqKey)
{
    const yandex::contest::system::execution::ProcessEnvironment environment = {
        {"incorrect=key", "value"}
    };
    BOOST_CHECK_THROW(ya::Exec("", {}, environment), ya::InvalidEnvironmentKeyError);
}

struct ExecFixture
{
    const boost::filesystem::path executable = "/some/path";
    const yandex::contest::system::execution::ProcessArguments fallbackArguments = {executable.string()};
    const yandex::contest::system::execution::ProcessArguments arguments = {"hello", "world"};
    const yandex::contest::system::execution::ProcessEnvironment environment = {
        {"PATH", "/bin:/usr/bin"},
        {"SHELL", "/bin/sh"},
        {"PWD", "/home"}};
};

BOOST_FIXTURE_TEST_CASE(ExecExecutable, ExecFixture)
{
    ya::Exec exec(executable);
    BOOST_CHECK_EQUAL(exec.executable(), executable.c_str());
    BOOST_CHECK_EQUAL_COLLECTIONS(exec.argv(), exec.argv() + fallbackArguments.size(),
                                  fallbackArguments.begin(), fallbackArguments.end());
    BOOST_CHECK(exec.argv()[fallbackArguments.size()] == nullptr);
    BOOST_CHECK(exec.envp()[0] == nullptr);
}

BOOST_FIXTURE_TEST_CASE(ExecExecutableArguments, ExecFixture)
{
    ya::Exec exec(executable, arguments);
    BOOST_CHECK_EQUAL(exec.executable(), executable.c_str());
    BOOST_CHECK_EQUAL_COLLECTIONS(exec.argv(), exec.argv() + arguments.size(),
                                  arguments.begin(), arguments.end());
    BOOST_CHECK(exec.argv()[arguments.size()] == nullptr);
    BOOST_CHECK(exec.envp()[0] == nullptr);
}

BOOST_FIXTURE_TEST_CASE(ExecExecutableArgumentsEnvironment, ExecFixture)
{
    ya::Exec exec(executable, arguments, environment);
    BOOST_CHECK_EQUAL(exec.executable(), executable.c_str());
    BOOST_CHECK_EQUAL_COLLECTIONS(exec.argv(), exec.argv() + arguments.size(),
                                  arguments.begin(), arguments.end());
    BOOST_CHECK(exec.argv()[arguments.size()] == nullptr);
    const std::vector<std::string> envp(exec.envp(), exec.envp() + environment.size());
    yandex::contest::system::execution::ProcessEnvironment environ;
    for (const std::string &s: envp)
    {
        const std::size_t eq = s.find('=');
        environ[s.substr(0, eq)] = s.substr(eq+1);
    }
    BOOST_CHECK(environ == environment);
}

BOOST_AUTO_TEST_SUITE_END()
