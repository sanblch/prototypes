#include "httpsession.h"
#include <boost/system/error_code.hpp>
#include <spdlog/spdlog.h>

HttpSession::HttpSession(Context &context, Configuration config)
  : strand_(boost::asio::make_strand(context)),
    config_{config},
    stream_(boost::asio::ip::tcp::socket(strand_)) {}

void HttpSession::start() {
  asyncRead();
}

void HttpSession::stop() {
  boost::system::error_code ec;
  stream_.socket().shutdown(Socket::shutdown_both, ec);
  boost::ignore_unused(ec);
}

auto HttpSession::makeBadResponse(std::string_view message,
                                  unsigned version,
                                  bool keep_alive) {
  Response res{boost::beast::http::status::bad_request, version};
  res.set(boost::beast::http::field::server, kServerName);
  res.set(boost::beast::http::field::content_type, "text/html");
  res.keep_alive(keep_alive);
  res.body() = message;
  res.prepare_payload();
  return res;
}

void HttpSession::handleRequest(Request&& req) {
  processRequest(req, shared_from_this());
}

void HttpSession::asyncRead() {
  parser_ = std::make_unique<Parser>();
  parser_->body_limit(config_.max_request_size);
  stream_.expires_after(config_.operation_timeout);

  boost::beast::http::async_read(
                                 stream_,
                                 buffer_,
                                 parser_->get(),
                                 [self = shared_from_this()](auto ec, auto count) {
                                   auto *s = dynamic_cast<HttpSession *>(self.get());
                                   BOOST_ASSERT_MSG(s != nullptr, "cannot cast to HttpSession");
                                   s->onRead(ec, count);
                                 });
}

void HttpSession::asyncWrite(Response message) {
  auto m = std::make_shared<Response>(std::forward<Response>(message));

  // write response
  boost::beast::http::async_write(
                                  stream_, *m, [self = shared_from_this(), m](auto ec, auto size) {
                                    self->onWrite(ec, size, m->need_eof());
                                  });
}

void HttpSession::respond(Response response) {
  return asyncWrite(response);
}

void HttpSession::onRead(boost::system::error_code ec, std::size_t) {
  if (ec) {
    if (HttpError::end_of_stream != ec) {
      reportError(ec, "unknown error occurred");
    }

    stop();
  }

  handleRequest(parser_->release());
}

void HttpSession::onWrite(boost::system::error_code ec,
                          std::size_t,
                          bool should_stop) {
  if (ec) {
    reportError(ec, "failed to write message");
    return stop();
  }

  if (should_stop) {
    return stop();
  }

  // read next request
  asyncRead();
}

void HttpSession::reportError(boost::system::error_code ec,
                              std::string_view message) {
  spdlog::error("error occured: {}, code: {}, message: {}", message, ec.value(), ec.message());
}
