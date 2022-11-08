#include <memory>
#include "../include/selfupdate/selfupdate.h"

namespace selfupdate {

PackageInfo query(std::string_view package_name,
                  std::string_view client_version) {
  return PackageInfo();
}

void download(const PackageInfo& package_url, DownloadProgressMonitor monitor) {

}

void install(const PackageInfo& PackageInfo) {}

}  // namespace selfupdate
