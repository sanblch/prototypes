#pragma once

#include "handler.h"
#include <prometheus/collectable.h>
#include <prometheus/counter.h>
#include <prometheus/family.h>
#include <prometheus/registry.h>
#include <prometheus/summary.h>
#include <memory>
#include <string_view>

class MetricsHandler : public Handler {
public:
  explicit MetricsHandler(prometheus::Registry& registry);

  void registerCollectable(const std::weak_ptr<prometheus::Collectable>& collectable);
  void removeCollectable(const std::weak_ptr<prometheus::Collectable>& collectable);

  void onSessionRequest(Request request, std::shared_ptr<Session> session);

 private:
  static void cleanupStalePointers(
                                   std::vector<std::weak_ptr<prometheus::Collectable>>& collectables);

  std::size_t writeResponse(std::shared_ptr<Session> session, Request request, const std::string& body);

  std::mutex collectables_mutex_;
  std::vector<std::weak_ptr<prometheus::Collectable>> collectables_;
  prometheus::Family<prometheus::Counter> &bytes_transferred_family_;
  prometheus::Counter &bytes_transferred_;
  prometheus::Family<prometheus::Counter> &num_scrapes_family_;
  prometheus::Counter &num_scrapes_;
  prometheus::Family<prometheus::Summary> &request_latencies_family_;
  prometheus::Summary &request_latencies_;
};
