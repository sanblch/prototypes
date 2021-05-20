#pragma once

#include "handler.h"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdint>

class Listener {
protected:
  using Acceptor = boost::asio::ip::tcp::acceptor;
  using Endpoint = boost::asio::ip::tcp::endpoint;

public:
  using Context = boost::asio::io_context;

  struct Configuration {
    Endpoint endpoint{};

    Configuration() {
      endpoint.address(boost::asio::ip::address_v4::any());
      endpoint.port(0);
    }
  };

  void setHandler(const std::shared_ptr<Handler>& handler) {
    handler_ = handler;
  }

  virtual ~Listener() = default;

  virtual bool prepare() = 0;
  virtual bool start() = 0;
  virtual void stop() = 0;
protected:
    /// Accept incoming connection
  virtual void acceptOnce() = 0;

  std::shared_ptr<Handler> handler_;
};
