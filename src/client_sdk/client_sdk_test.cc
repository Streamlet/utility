#include <iostream>
#include <math.h>
#include <selfupdate/selfupdate.h>

int main() {
  selfupdate::Initialize("selfupdate_test");

  std::cout << "Step 1: query package info" << std::endl;
  selfupdate::PackageInfo package_info;
  std::error_code ec = selfupdate::Query("http://localhost:8080/query", "", package_info);
  if (ec) {
    std::cout << ec.value() << ": " << ec.message() << std::endl;
    return -1;
  }
  std::cout << "package_name: " << package_info.package_name << std::endl;
  std::cout << "package_version: " << package_info.package_version << std::endl;
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

  return 0;
}
