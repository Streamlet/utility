#include "http_client.h"
#include <iostream>

void on_response(unsigned status, HttpClient::HttpHeader header,
                 std::string_view body) {
  std::cout << status << std::endl;
  for (auto &h : header) {
    std::cout << h.first << ": " << h.second << std::endl;
  }
  std::cout << body << std::endl;
}

void on_error(std::error_code ec, const char *what) {
  std::cout << what << ":" << ec << std::endl;
}

int main(int argc, char *argv[]) {
  HttpClient http(
      "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
      "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/107.0.0.0 Safari/537.36");
  if (argc <= 1)
    return -1;
  http.Get(argv[1], {}, on_response, on_error);
  http.Wait();
  return 0;
}
