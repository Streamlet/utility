#include <iostream>
#include <xl/http_client>
#include <xl/process>

int main(int argc, char *argv[]) {
  HttpClient http;
  if (argc <= 1) {
    return -1;
  }

  unsigned status = 0;
  HttpClient::ResponseHeader header;
  std::string body;
  std::error_code ec = {};
  const int RETRY_TIMES = 3;
  for (int i = 0; i < RETRY_TIMES; ++i) {
    ec = http.Get(argv[1], {}, &status, &header, &body);
    if (!ec) {
      break;
    }
    std::cout << ec.value() << ": " << ec.message().c_str() << (i + i < RETRY_TIMES ? ", retrying..." : ", failed")
              << std::endl;
    xl::process::sleep(1000);
  }
  if (ec) {
    return -1;
  }

  std::cout << status << std::endl;
  for (auto &h : header) {
    std::cout << h.first.c_str() << ": " << h.second.c_str() << std::endl;
  }
  std::cout << body.c_str() << std::endl;

  return 0;
}
