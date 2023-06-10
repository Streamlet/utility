#include <boost/dll/runtime_symbol_info.hpp>
#include <iostream>
#include <math.h>
#include <selfupdate/installer.h>
#include <selfupdate/selfupdate.h>

#ifdef _WIN
const char *NEW_CLIENT_NAME = "new_client.exe";
#else
const char *NEW_CLIENT_NAME = "new_client";
#endif

int main(int argc, const char *argv[]) {

  selfupdate::InstallContext install_context;
  if (selfupdate::IsInstallMode(argc, argv, install_context)) {
    std::cout << "Install from " << install_context.source << " to " << install_context.target << std::endl;
    std::error_code ec = selfupdate::DoInstall(install_context);
    if (ec) {
      std::cout << ec.value() << ": " << ec.message() << std::endl;
      return -1;
    }
    return 0;
  }

  std::cout << "This is old client." << std::endl << std::endl;

  std::cout << "Step 1: query package info" << std::endl;
  selfupdate::PackageInfo package_info;
  std::error_code ec = selfupdate::Query("http://localhost:8080/example/1.0", "", package_info);
  if (ec) {
    std::cout << ec.value() << ": " << ec.message() << std::endl;
    return -1;
  }
  std::cout << "package_name: " << package_info.package_name << std::endl;
  std::cout << "has_new_version: " << package_info.has_new_version << std::endl;
  std::cout << "package_version: " << package_info.package_version << std::endl;
  std::cout << "force_update: " << package_info.force_update << std::endl;
  std::cout << "package_url: " << package_info.package_url << std::endl;
  std::cout << "package_size: " << package_info.package_size << std::endl;
  std::cout << "package_format: " << package_info.package_format << std::endl;
  std::cout << "package_hash: " << std::endl;
  for (const auto &item : package_info.package_hash)
    std::cout << "  " << item.first << ": " << item.second << std::endl;
  std::cout << "update_title: " << package_info.update_title << std::endl;
  std::cout << "update_description: " << package_info.update_description << std::endl;

  std::cout << "Step 2: download package" << std::endl;
  ec = selfupdate::Download(package_info, [](unsigned long long downloaded_bytes, unsigned long long total_bytes) {
    std::cout << (round(downloaded_bytes * 10000.0 / total_bytes) / 100) << "%, " << downloaded_bytes << "/"
              << total_bytes << std::endl;
  });

  if (ec) {
    std::cout << ec.value() << ": " << ec.message() << std::endl;
    return -1;
  }

  std::cout << "Step 3: install package" << std::endl;
  ec = selfupdate::Install(package_info, boost::dll::program_location().string(), {}, NEW_CLIENT_NAME);
  if (ec) {
    std::cout << ec.value() << ": " << ec.message() << std::endl;
    return -1;
  }

  return 0;
}
