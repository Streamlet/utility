#include "http_client.h"
#include "url.h"
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

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
namespace ssl = net::ssl;       // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace {

const unsigned HTTP_VERSION = 11;
const char *PROTOCOL_HTTPS = "https";
const char *PROTOCOL_HTTP = "http";
const char *DEFAULT_PORT_HTTPS = "443";
const char *DEFAULT_PORT_HTTP = "80";
const unsigned CLOSE_CONNECTION_MAX_TIMEOUT = 1000;

class HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
  HttpSession(net::io_context &ioc, std::string user_agent, std::string url,
              http::verb method, HttpClient::HttpHeader header,
              std::string body, HttpClient::HttpResponseHandler on_response,
              HttpClient::HttpErrorNotifier on_error, unsigned timeout)
      : ioc_(ioc), on_response_(std::move(on_response)),
        on_error_(std::move(on_error)),
        timeout_(timeout), ctx_{ssl::context::tlsv12_client},
        resolver_(net::make_strand(ioc_)), tcp_stream_(net::make_strand(ioc_)),
        ssl_stream_(net::make_strand(ioc_), ctx_) {
    // This holds the root certificate used for verification
    // load_root_certificates(ctx_);
    // Verify the remote server's certificate
    ctx_.set_verify_mode(ssl::verify_peer);

    url_parts_.parse(std::move(url));
    request_.method(method);
    request_.target(!url_parts_.full_path.empty() ? url_parts_.full_path : "/");
    request_.version(HTTP_VERSION);
    request_.set(http::field::host, url_parts_.domain);
    request_.set(http::field::user_agent, !user_agent.empty()
                                              ? std::move(user_agent)
                                              : BOOST_BEAST_VERSION_STRING);
    for (auto &h : header) {
      request_.set(h.first, h.second);
    }
    request_.body() = std::move(body);
  }
  ~HttpSession() {}

  bool start() {
    if (url_parts_.protocol == PROTOCOL_HTTPS) {
      return resolve_ssl();
    } else if (url_parts_.protocol == PROTOCOL_HTTP) {
      resolve();
    } else {
      return false;
    }

    return true;
  }

private:
  bool resolve_ssl() {
    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(ssl_stream_.native_handle(),
                                  url_parts_.domain.data())) {
      return false;
    }

    // Look up the domain name
    std::string_view port =
        !url_parts_.port.empty() ? url_parts_.port : DEFAULT_PORT_HTTPS;
    resolver_.async_resolve(
        url_parts_.domain, port,
        beast::bind_front_handler(&HttpSession::on_resolve_ssl,
                                  shared_from_this()));
    return true;
  }
  void resolve() {
    // Look up the domain name
    std::string_view port =
        !url_parts_.port.empty() ? url_parts_.port : DEFAULT_PORT_HTTP;
    resolver_.async_resolve(url_parts_.domain, port,
                            beast::bind_front_handler(&HttpSession::on_resolve,
                                                      shared_from_this()));
  }

  void on_resolve_ssl(beast::error_code ec,
                      tcp::resolver::results_type results) {
    if (ec)
      return error(ec, "resolve");

    // Set a timeout on the operation
    if (timeout_ > 0) {
      beast::get_lowest_layer(ssl_stream_)
          .expires_after(std::chrono::milliseconds(timeout_));
    }
    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(ssl_stream_)
        .async_connect(results,
                       beast::bind_front_handler(&HttpSession::on_connect_ssl,
                                                 shared_from_this()));
  }
  void on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec)
      return error(ec, "resolve");

    // Set a timeout on the operation
    if (timeout_ > 0) {
      beast::get_lowest_layer(ssl_stream_)
          .expires_after(std::chrono::milliseconds(timeout_));
    }
    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(tcp_stream_)
        .async_connect(results,
                       beast::bind_front_handler(&HttpSession::on_connect,
                                                 shared_from_this()));
  }

  void on_connect_ssl(beast::error_code ec,
                      tcp::resolver::results_type::endpoint_type) {
    if (ec)
      return error(ec, "connect");

    // Perform the SSL handshake
    ssl_stream_.async_handshake(
        ssl::stream_base::client,
        beast::bind_front_handler(&HttpSession::on_ssl_handshake,
                                  shared_from_this()));
  }

  void on_connect(beast::error_code ec,
                  tcp::resolver::results_type::endpoint_type) {
    if (ec)
      return error(ec, "connect");

    // Set a timeout on the operation
    if (timeout_ > 0) {
      beast::get_lowest_layer(tcp_stream_)
          .expires_after(std::chrono::milliseconds(timeout_));
    }

    // Send the HTTP request to the remote host
    http::async_write(
        tcp_stream_, request_,
        beast::bind_front_handler(&HttpSession::on_write, shared_from_this()));
  }

  void on_ssl_handshake(beast::error_code ec) {
    if (ec)
      return error(ec, "handshake");

    // Set a timeout on the operation
    if (timeout_ > 0) {
      beast::get_lowest_layer(ssl_stream_)
          .expires_after(std::chrono::milliseconds(timeout_));
    }

    // Send the HTTP request to the remote host
    http::async_write(ssl_stream_, request_,
                      beast::bind_front_handler(&HttpSession::on_write_ssl,
                                                shared_from_this()));
  }

  void on_write_ssl(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec)
      return error(ec, "write");

    // Receive the HTTP response
    http::async_read(ssl_stream_, buffer_, response_parser_,
                     beast::bind_front_handler(&HttpSession::on_read_ssl,
                                               shared_from_this()));
  }
  void on_write(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec)
      return error(ec, "write");

    // Receive the HTTP response
    http::async_read(
        tcp_stream_, buffer_, response_parser_,
        beast::bind_front_handler(&HttpSession::on_read, shared_from_this()));
  }

  void on_read_ssl(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if (ec)
      return error(ec, "read_body");

    response_ = response_parser_.get();

    // Set a timeout on the operation
    beast::get_lowest_layer(ssl_stream_)
        .expires_after(std::chrono::milliseconds(
            timeout_ > 0 && timeout_ < CLOSE_CONNECTION_MAX_TIMEOUT
                ? timeout_
                : CLOSE_CONNECTION_MAX_TIMEOUT));
    // Gracefully close the stream
    ssl_stream_.async_shutdown(beast::bind_front_handler(
        &HttpSession::on_shutdown, shared_from_this()));
  }
  void on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if (ec)
      return error(ec, "read_body");

    response_ = response_parser_.get();

    // Set a timeout on the operation
    beast::get_lowest_layer(tcp_stream_)
        .expires_after(std::chrono::milliseconds(
            timeout_ > 0 && timeout_ < CLOSE_CONNECTION_MAX_TIMEOUT
                ? timeout_
                : CLOSE_CONNECTION_MAX_TIMEOUT));
    // Gracefully close the stream
    tcp_stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
    HttpSession::on_shutdown(ec);
  }

  void on_shutdown(beast::error_code ec) {
    if (ec == net::error::eof) {
      // Rationale:
      // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
      ec = {};
    }
    // if (ec)
    //   return error(ec, "shutdown");

    // If we get here then the connection is closed gracefully
    callback();
  }

  void error(beast::error_code ec, const char *what) {
    if (!on_error_)
      return;
    on_error_(ec, what);
  }

  void callback() {
    if (!on_response_)
      return;
    HttpClient::HttpHeader header;
    for (auto &h : response_) {
      header.insert({h.name_string(), h.value()});
    }
    on_response_(response_.result_int(), std::move(header), response_.body());
  }

private:
  unsigned timeout_ = 0;
  HttpClient::HttpResponseHandler on_response_;
  HttpClient::HttpErrorNotifier on_error_;

  Url url_parts_;
  http::request<http::string_body> request_;

  net::io_context &ioc_;
  ssl::context ctx_;
  tcp::resolver resolver_;
  beast::ssl_stream<beast::tcp_stream> ssl_stream_;
  beast::tcp_stream tcp_stream_;
  beast::flat_buffer buffer_; // (Must persist between reads)
  http::response_parser<http::string_body> response_parser_;
  http::response<http::string_body> response_;
};

bool start_session(net::io_context &ioc, std::string user_agent,
                   std::string url, http::verb method,
                   HttpClient::HttpHeader header, std::string body,
                   HttpClient::HttpResponseHandler on_response,
                   HttpClient::HttpErrorNotifier on_error, unsigned timeout) {
  auto session = std::make_shared<HttpSession>(
      ioc, user_agent, std::move(url), http::verb::get, std::move(header), "",
      std::move(on_response), std::move(on_error), timeout);
  return session->start();
}

} // namespace

HttpClient::HttpClient(std::string user_agent /*= ""*/)
    : user_agent_(std::move(user_agent)),
      waitable_context_(new net::io_context) {}

HttpClient::~HttpClient() {
  Wait();
  delete reinterpret_cast<net::io_context *>(waitable_context_);
}

bool HttpClient::Get(std::string url, HttpHeader header,
                     HttpResponseHandler on_response,
                     HttpErrorNotifier on_error, unsigned timeout /*= 0*/) {
  return start_session(*reinterpret_cast<net::io_context *>(waitable_context_),
                       user_agent_, url, http::verb::get, std::move(header), "",
                       std::move(on_response), std::move(on_error), timeout);
}

bool HttpClient::Post(std::string url, HttpHeader header, std::string body,
                      HttpResponseHandler on_response,
                      HttpErrorNotifier on_error, unsigned timeout /*= 0*/) {
  return start_session(*reinterpret_cast<net::io_context *>(waitable_context_),
                       user_agent_, url, http::verb::post, std::move(header),
                       std::move(body), std::move(on_response),
                       std::move(on_error), timeout);
}

bool HttpClient::Put(std::string url, HttpHeader header, std::string body,
                     HttpResponseHandler on_response,
                     HttpErrorNotifier on_error, unsigned timeout /*= 0*/) {
  return start_session(*reinterpret_cast<net::io_context *>(waitable_context_),
                       user_agent_, url, http::verb::put, std::move(header),
                       std::move(body), std::move(on_response),
                       std::move(on_error), timeout);
}

bool HttpClient::Delete(std::string url, HttpHeader header,
                        HttpResponseHandler on_response,
                        HttpErrorNotifier on_error, unsigned timeout /*= 0*/) {
  return start_session(*reinterpret_cast<net::io_context *>(waitable_context_),
                       user_agent_, url, http::verb::delete_, std::move(header),
                       "", std::move(on_response), std::move(on_error),
                       timeout);
}

void HttpClient::Wait() {
  reinterpret_cast<net::io_context *>(waitable_context_)->run();
}
