#define BOOST_TEST_MODULE cgroup_ProcPidCgroup
#include <boost/test/unit_test.hpp>

#include <yandex/contest/system/cgroup/ProcPidCgroup.hpp>

#include <yandex/contest/Tempfile.hpp>

namespace ya = yandex::contest::system;
namespace yac = ya::cgroup;

BOOST_AUTO_TEST_SUITE(ProcPidCgroup)

BOOST_AUTO_TEST_CASE(empty) {
  yandex::contest::Tempfile tmp("");
  yac::ProcPidCgroup ppc;
  ppc.load(tmp.path());
  BOOST_CHECK(ppc.entries.empty());
}

BOOST_AUTO_TEST_CASE(load) {
  yandex::contest::Tempfile tmp(
      "7:freezer:/some/freezer/path\n"
      "4:name=systemd:/some/named/path\n"
      "3:devices:/some/dev/path\n"
      "2:cpuacct,cpu:/\n");
  yac::ProcPidCgroup ppc;
  ppc.load(tmp.path());
  BOOST_REQUIRE_EQUAL(ppc.entries.size(), 4);
  BOOST_CHECK_EQUAL(ppc.entries[0].hierarchyId, 7);
  BOOST_CHECK_EQUAL(ppc.entries[0].controlGroup, "/some/freezer/path");
  BOOST_CHECK_EQUAL(ppc.entries[0].subsystems.size(), 1);
  BOOST_CHECK(ppc.entries[0].subsystems.find("freezer") !=
              ppc.entries[0].subsystems.end());

  BOOST_CHECK_EQUAL(ppc.entries[1].hierarchyId, 4);
  BOOST_CHECK_EQUAL(ppc.entries[1].controlGroup, "/some/named/path");
  BOOST_CHECK_EQUAL(ppc.entries[1].subsystems.size(), 1);
  BOOST_CHECK(ppc.entries[1].subsystems.find("name=systemd") !=
              ppc.entries[1].subsystems.end());

  BOOST_CHECK_EQUAL(ppc.entries[2].hierarchyId, 3);
  BOOST_CHECK_EQUAL(ppc.entries[2].controlGroup, "/some/dev/path");
  BOOST_CHECK_EQUAL(ppc.entries[2].subsystems.size(), 1);
  BOOST_CHECK(ppc.entries[2].subsystems.find("devices") !=
              ppc.entries[2].subsystems.end());

  BOOST_CHECK_EQUAL(ppc.entries[3].hierarchyId, 2);
  BOOST_CHECK_EQUAL(ppc.entries[3].controlGroup, "/");
  BOOST_CHECK_EQUAL(ppc.entries[3].subsystems.size(), 2);
  BOOST_CHECK(ppc.entries[3].subsystems.find("cpu") !=
              ppc.entries[3].subsystems.end());
  BOOST_CHECK(ppc.entries[3].subsystems.find("cpuacct") !=
              ppc.entries[3].subsystems.end());
}

BOOST_AUTO_TEST_SUITE_END()  // ProcPidCgroup
