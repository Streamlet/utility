#include <iostream>
#include <xl/http>
#include <xl/process>

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    return -1;
  }
  const char *url = argv[1];

  xl::http::Header header;
  std::string body;
  int status = 0;
  const int RETRY_TIMES = 3;
  for (int i = 0; i < RETRY_TIMES; ++i) {
    status = xl::http::get(url, {}, header, xl::http::BufferWriter(&body));
    if (status >= 0) {
      break;
    }
    std::cout << "Error: " << status << ", " << (i + i < RETRY_TIMES ? ", retrying..." : ", failed") << std::endl;
    xl::process::sleep(1000);
  }
  if (status < 0) {
    return -1;
  }

  std::cout << status << std::endl;
  for (auto &h : header) {
    std::cout << h.first.c_str() << ": " << h.second.c_str() << std::endl;
  }
  std::cout << body.c_str() << std::endl;

  return 0;
}
