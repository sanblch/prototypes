#pragma once

#include "session.h"

class Handler {
public:
  using Body = boost::beast::http::string_body;
  using Request = boost::beast::http::request<Body>;
  using Response = boost::beast::http::response<Body>;
  virtual void onSessionRequest(Request request,
                                std::shared_ptr<Session> session) = 0;
};
