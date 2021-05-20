#include "httplistener.h"
#include "listener.h"
#include "metricshandler.h"
#include <prometheus/registry.h>
#include <cstdlib>
#include <iostream>
#include <memory>

int main() {
  try {
    auto registry = std::make_shared<prometheus::Registry>();
    auto handler = std::make_shared<MetricsHandler>(*registry.get());
    handler->registerCollectable(registry);

    auto ctx = std::make_shared<boost::asio::io_context>();
    Listener::Configuration lconf;
    lconf.endpoint.address(boost::asio::ip::make_address_v4("127.0.0.1"));
    lconf.endpoint.port(8888);
    HttpSession::Configuration sconf;

    auto listener = std::make_shared<HttpListenerImpl>(ctx, lconf, sconf);
    listener->setHandler(std::dynamic_pointer_cast<Handler>(handler));
    listener->prepare();
    listener->start();

    ctx->run();
  } catch(std::exception const& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
