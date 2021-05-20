#include "httpsession.h"
#include "handler.h"
#include "listener.h"

class HttpListenerImpl
  : public Listener,
    public std::enable_shared_from_this<HttpListenerImpl> {
public:
  using SessionImpl = HttpSession;

  HttpListenerImpl(
                   std::shared_ptr<Context> context,
                   Configuration listener_config,
                   SessionImpl::Configuration session_config);

  ~HttpListenerImpl() override = default;

  void setHandler(const std::shared_ptr<Handler>& handler) {
    handler_ = handler;
  }

  bool prepare() override;
  bool start() override;
  void stop() override;

private:
  void acceptOnce() override;

  std::shared_ptr<Context> context_;
  const Configuration config_;
  std::shared_ptr<Handler> handler_;
  const SessionImpl::Configuration session_config_;

  std::unique_ptr<Acceptor> acceptor_;

  std::shared_ptr<SessionImpl> new_session_;
};
