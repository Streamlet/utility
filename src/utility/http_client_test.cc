#include "http_client.h"
#include <iostream>

int main(int argc, char *argv[]) {
  HttpClient http;
  if (argc <= 1)
    return -1;

  unsigned status = 0;
  HttpClient::ResponseHeader header;
  std::string body;
  auto ec = http.Get(argv[1], {}, &status, &header, &body);
  if (ec) {
    std::cout << ec.value() << ": " << ec.message().c_str() << std::endl;
    return -1;
  }

  std::cout << status << std::endl;
  for (auto &h : header) {
    std::cout << h.first.c_str() << ": " << h.second.c_str() << std::endl;
  }
  std::cout << body.c_str() << std::endl;

  return 0;
}
