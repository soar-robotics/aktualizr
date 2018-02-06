#include "deb.h"

#define LIBDPKG_VOLATILE_API 1
#include <dpkg/dpkg-db.h>
#include <dpkg/dpkg.h>
#include <dpkg/pkg-array.h>
#include <dpkg/pkg-show.h>
#include <stdio.h>
#include <unistd.h>

Json::Value DebianManager::getInstalledPackages() {
  Json::Value packages(Json::arrayValue);
  struct pkg_array array;
  dpkg_program_init("a.out");
  modstatdb_open(msdbrw_available_readonly);

  pkg_array_init_from_db(&array);
  pkg_array_sort(&array, pkg_sorter_by_nonambig_name_arch);
  for (int i = 0; i < array.n_pkgs; ++i) {
    if (array.pkgs[i]->status == PKG_STAT_INSTALLED) {
      Json::Value package;
      package["name"] = pkg_name(array.pkgs[i], pnaw_nonambig);
      package["version"] = versiondescribe(&array.pkgs[i]->installed.version, vdew_nonambig);
      packages.append(package);
    }
  }
  dpkg_program_done();
  return packages;
}

data::InstallOutcome DebianManager::install(const Uptane::Target &target) const {
  std::string cmd = "dpkg -i ";
  std::string output;
  int status = Utils::shell(cmd + (path_ / "targets" / target.filename()).string(), &output);
  if (status != 0) {
    return data::InstallOutcome(data::INSTALL_FAILED, output);
  }
  Utils::writeFile(path_ / "targets/installed", target.sha256Hash());
  return data::InstallOutcome(data::OK, "Installing debian package was successful");
}

std::string DebianManager::getCurrent() { return Utils::readFile(path_ / "targets/installed"); }