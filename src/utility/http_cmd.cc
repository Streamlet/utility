#include "http_client.h"
#include <iostream>
#include <Windows.h>

int main(int argc, char *argv[]) {
  HttpClient http;
  if (argc <= 1)
    return -1;

  unsigned status = 0;
  HttpClient::ResponseHeader header;
  std::string body;
  auto ec = http.Get(argv[1], {}, &status, &header, &body);
  if (ec) {
    std::cout << ec << std::endl;
    return -1;
  }

  std::cout << status << std::endl;
  for (auto &h : header) {
    std::cout << h.first << ": " << h.second << std::endl;
  }
  std::cout << body << std::endl;

  return 0;
}
