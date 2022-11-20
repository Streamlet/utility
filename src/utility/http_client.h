#include <functional>
#include <map>
#include <memory>
#include <string_view>
#include <system_error>

class HttpClient {
public:
  HttpClient(std::string user_agent = "");
  ~HttpClient();

  typedef std::multimap<std::string_view, std::string_view> HttpHeader;

  typedef std::function<void(unsigned status, HttpHeader header,
                             std::string_view body)>
      HttpResponseHandler;
  typedef std::function<void(const char *error)>
      HttpErrorNotifier;

  bool Get(std::string url, HttpHeader header, HttpResponseHandler on_response,
           HttpErrorNotifier on_error, unsigned timeout = 0);
  bool Post(std::string url, HttpHeader header, std::string body,
            HttpResponseHandler on_response, HttpErrorNotifier on_error,
            unsigned timeout = 0);
  bool Put(std::string url, HttpHeader header, std::string body,
           HttpResponseHandler on_response, HttpErrorNotifier on_error,
           unsigned timeout = 0);
  bool Delete(std::string url, HttpHeader header,
              HttpResponseHandler on_response, HttpErrorNotifier on_error,
              unsigned timeout = 0);

  void Wait();

private:
  std::string user_agent_;
  void *waitable_context_ = nullptr;
};
