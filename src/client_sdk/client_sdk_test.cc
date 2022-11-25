#include <iostream>
#include <selfupdate/selfupdate.h>

int main() {
  selfupdate::PackageInfo package_info;
  std::error_code ec = selfupdate::Query("http://localhost:8080/query", "", package_info);
  if (ec) {
    std::cout << ec << std::endl;
    return -1;
  }
  std::cout << "title: " << package_info.title << std::endl;
  std::cout << "description: " << package_info.description << std::endl;
  std::cout << "server_version: " << package_info.server_version << std::endl;
  std::cout << "package_url: " << package_info.package_url << std::endl;
  std::cout << "package_format: " << package_info.package_format << std::endl;
  return 0;
}