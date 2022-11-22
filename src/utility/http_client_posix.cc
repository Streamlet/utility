#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/version.hpp>
#include <iostream>
#include <memory>

#include "http_client.h"
#include "url.h"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http  = beast::http;  // from <boost/beast/http.hpp>
namespace net   = boost::asio;  // from <boost/asio.hpp>
namespace ssl   = net::ssl;     // from <boost/asio/ssl.hpp>
using tcp       = net::ip::tcp; // from <boost/asio/ip/tcp.hpp>

namespace {

const unsigned HTTP_VERSION                 = 11;
const char *PROTOCOL_HTTPS                  = "https";
const char *PROTOCOL_HTTP                   = "http";
const char *INTERNET_DEFAULT_HTTPS_PORT     = "443";
const char *INTERNET_DEFAULT_HTTP_PORT      = "80";
const unsigned CLOSE_CONNECTION_MAX_TIMEOUT = 1000;

} // namespace

class HttpClient::HttpSession {
public:
  HttpSession(const std::string &user_agent) : user_agent_(std::move(user_agent)), resolver_(net::make_strand(ioc_)) {
  }
  ~HttpSession() {
  }
  std::error_code SendAndReceive(http::verb method,
                                 const std::string_view &url_string,
                                 const RequestHeader &request_header,
                                 const std::string_view &request_body,
                                 unsigned *response_status,
                                 ResponseHeader *response_header,
                                 std::string *response_body,
                                 unsigned timeout = 0) {
    Url url = Url::Parse(url_string);
    if (!url.valid)
      return std::make_error_code(std::errc::invalid_argument);
    if (url.protocol != PROTOCOL_HTTPS && url.protocol != PROTOCOL_HTTP)
      return std::make_error_code(std::errc::protocol_not_supported);
    bool ssl = url.protocol == PROTOCOL_HTTPS;

    std::string_view port = url.port;
    if (port.empty())
      port = ssl ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT;

    boost::system::error_code ec;
    tcp::resolver::results_type resolve_results = Resolve(url.domain, port, ec);
    if (ec)
      return ec;

    http::request<http::string_body> request;
    request.method(method);
    request.target(!url.full_path.empty() ? url.full_path : "/");
    request.version(HTTP_VERSION);
    request.set(http::field::host, url.domain);
    request.set(http::field::user_agent, !user_agent_.empty() ? std::move(user_agent_) : BOOST_BEAST_VERSION_STRING);
    for (auto &h : request_header) {
      request.set(h.first, h.second);
    }
    request.body() = std::move(request_body);

    beast::flat_buffer buffer; // (Must persist between reads)
    http::response_parser<http::string_body> response_parser;
    if (ssl) {
      ec = SendAndReceiveHttps(url.domain, resolve_results, request, buffer, response_parser, timeout);
    } else {
      ec = SendAndReceiveHttp(resolve_results, request, buffer, response_parser, timeout);
    }
    if (ec)
      return ec;

    http::response<http::string_body> response = response_parser.get();
    if (response_status != nullptr) {
      *response_status = response.result_int();
    }
    if (response_header != nullptr) {
      for (auto &h : response) {
        response_header->insert(std::make_pair(h.name_string(), h.value()));
      }
    }
    if (response_body != nullptr) {
      *response_body = response.body();
    }
    return {};
  }

private:
  tcp::resolver::results_type
  Resolve(const std::string_view &host, const std::string_view &port, boost::system::error_code &ec) {
    return resolver_.resolve(host, port, ec);
  }

  std::error_code SendAndReceiveHttps(const std::string_view &host,
                                      const tcp::resolver::results_type &resolve_results,
                                      const http::request<http::string_body> &request,
                                      beast::flat_buffer &buffer,
                                      http::response_parser<http::string_body> &response_parser,
                                      unsigned timeout = 0) {
    ssl::context ssl_ctx{ssl::context::tlsv12_client};
    beast::ssl_stream<beast::tcp_stream> ssl_stream(net::make_strand(ioc_), ssl_ctx);
    // This holds the root certificate used for verification
    // load_root_certificates(ssl_ctx);
    // Verify the remote server's certificate
    ssl_ctx.set_verify_mode(ssl::verify_peer);
    // Set SNI Hostname (many hosts need this to handshake successfully)
    SSL_set_tlsext_host_name(ssl_stream.native_handle(), host.data());

    boost::system::error_code ec;

    SetTimeout(ssl_stream, timeout);
    Connect(ssl_stream, resolve_results, ec);
    if (ec)
      return ec;

    SetTimeout(ssl_stream, timeout);
    HandShake(ssl_stream, ec);
    if (ec)
      return ec;

    SetTimeout(ssl_stream, timeout);
    size_t bytes_written = WriteRequest(ssl_stream, request, ec);
    if (ec)
      return ec;

    SetTimeout(ssl_stream, timeout);
    size_t bytes_read = ReadResponse(ssl_stream, buffer, response_parser, ec);
    if (ec)
      return ec;

    SetTimeout(ssl_stream,
               timeout > 0 && timeout < CLOSE_CONNECTION_MAX_TIMEOUT ? timeout : CLOSE_CONNECTION_MAX_TIMEOUT);
    ssl_stream.shutdown(ec);

    return {};
  }

  std::error_code SendAndReceiveHttp(const tcp::resolver::results_type &resolve_results,
                                     const http::request<http::string_body> &request,
                                     beast::flat_buffer &buffer,
                                     http::response_parser<http::string_body> &response_parser,
                                     unsigned timeout = 0) {
    beast::tcp_stream tcp_stream(net::make_strand(ioc_));

    boost::system::error_code ec;

    SetTimeout(tcp_stream, timeout);
    Connect(tcp_stream, resolve_results, ec);
    if (ec)
      return ec;

    SetTimeout(tcp_stream, timeout);
    size_t bytes_transferred = WriteRequest(tcp_stream, request, ec);
    if (ec)
      return ec;

    SetTimeout(tcp_stream, timeout);
    size_t bytes_read = ReadResponse(tcp_stream, buffer, response_parser, ec);
    if (ec)
      return ec;

    tcp_stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    return {};
  }

  template <typename StreamType>
  void SetTimeout(StreamType &stream, unsigned timeout) {
    if (timeout > 0) {
      beast::get_lowest_layer(stream).expires_after(std::chrono::milliseconds(timeout));
    }
  }

  template <typename StreamType>
  void Connect(StreamType &stream, const tcp::resolver::results_type &resolve_results, boost::system::error_code &ec) {
    tcp::resolver::results_type::endpoint_type ep = beast::get_lowest_layer(stream).connect(resolve_results, ec);
  }

  void HandShake(beast::ssl_stream<beast::tcp_stream> &ssl_stream, beast::error_code &ec) {
    ssl_stream.handshake(ssl::stream_base::client, ec);
  }

  template <typename StreamType>
  std::size_t
  WriteRequest(StreamType &stream, const http::request<http::string_body> &request, boost::system::error_code &ec) {
    return http::write(stream, request, ec);
  }

  template <typename StreamType>
  std::size_t ReadResponse(StreamType &stream,
                           beast::flat_buffer &buffer,
                           http::response_parser<http::string_body> &response_parser,
                           boost::system::error_code &ec) {
    return http::read(stream, buffer, response_parser, ec);
  }

private:
  std::string user_agent_;
  net::io_context ioc_;
  tcp::resolver resolver_;
};

HttpClient::HttpClient(std::string user_agent) : session_(std::make_unique<HttpSession>(std::move(user_agent))) {
}

HttpClient::~HttpClient() {
}

std::error_code HttpClient::Get(const std::string_view &url,
                                const RequestHeader &request_header,
                                unsigned *response_status,
                                ResponseHeader *response_header,
                                std::string *response_body,
                                unsigned timeout) {
  return session_->SendAndReceive(http::verb::get, url, request_header, "", response_status, response_header,
                                  response_body);
}

std::error_code HttpClient::Post(const std::string_view &url,
                                 const RequestHeader &request_header,
                                 const std::string_view &request_body,
                                 unsigned *response_status,
                                 ResponseHeader *response_header,
                                 std::string *response_body,
                                 unsigned timeout) {
  return session_->SendAndReceive(http::verb::post, url, request_header, request_body, response_status, response_header,
                                  response_body);
}

std::error_code HttpClient::Put(const std::string_view &url,
                                const RequestHeader &request_header,
                                const std::string_view &request_body,
                                unsigned *response_status,
                                ResponseHeader *response_header,
                                std::string *response_body,
                                unsigned timeout) {
  return session_->SendAndReceive(http::verb::put, url, request_header, request_body, response_status, response_header,
                                  response_body);
}

std::error_code HttpClient::Delete(const std::string_view &url,
                                   const RequestHeader &request_header,
                                   unsigned *response_status,
                                   ResponseHeader *response_header,
                                   std::string *response_body,
                                   unsigned timeout) {
  return session_->SendAndReceive(http::verb::delete_, url, request_header, "", response_status, response_header,
                                  response_body);
}
