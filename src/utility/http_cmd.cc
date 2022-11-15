#include "http_client.h"

int main(int argc, char *argv[]) {
  HttpClient http(
      "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
      "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/107.0.0.0 Safari/537.36");
  if (argc <= 1)
    return -1;
  http.Get(argv[1], {}, nullptr, nullptr);
  http.Wait();
  return 0;
}
