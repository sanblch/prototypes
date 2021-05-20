#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/beast/http/string_body.hpp>

class Session {
  using Body = boost::beast::http::string_body;
  using Request = boost::beast::http::request<Body>;
  using Response = boost::beast::http::response<Body>;
  using OnRequestSignature = void(Request, std::shared_ptr<Session> session);
  std::function<OnRequestSignature> on_request_; ///< `on request` callback
public:
  using Context = boost::asio::io_context;
  using Socket = boost::asio::ip::tcp::socket;
  using ErrorCode = boost::system::error_code;
  using Streambuf = boost::asio::streambuf;
  using Timer = boost::asio::steady_timer;
  using Duration = Timer::duration;
  using SessionId = uint64_t;

  virtual ~Session() = default;

  virtual void start() = 0;

  virtual Socket &socket() = 0;

  virtual SessionId id() const = 0;

  void connectOnRequest(std::function<OnRequestSignature> callback) {
    on_request_ = std::move(callback);
  }

  void processRequest(Request request,
                      std::shared_ptr<Session> session) {
    on_request_(request, std::move(session));
  }

  virtual void respond(Response message) = 0;
};
