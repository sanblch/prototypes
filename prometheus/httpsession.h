#pragma once

#include "session.h"
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <string_view>

class HttpSession : public Session,
                    public std::enable_shared_from_this<HttpSession> {
    using Body = boost::beast::http::string_body;
    using Request = boost::beast::http::request<Body>;
    using Response = boost::beast::http::response<Body>;
    using Parser = boost::beast::http::request_parser<Body>;
    using HttpField = boost::beast::http::field;
    using HttpError = boost::beast::http::error;

   public:
    struct Configuration {
      static constexpr size_t kDefaultRequestSize = 10000u;
      static constexpr Duration kDefaultTimeout = std::chrono::seconds(30);

      size_t max_request_size{kDefaultRequestSize};
      Duration operation_timeout{kDefaultTimeout};
    };

    ~HttpSession() override = default;

    /**
     * @brief constructor
     * @param socket socket instance
     * @param config session configuration
     */
    HttpSession(Context &context, Configuration config);

    Socket &socket() override {
      return stream_.socket();
    }

    /**
     * @brief starts session
     */
    void start() override;

    /**
     * @brief sends response wrapped by http message
     * @param response message to send
     */
    void respond(Response response) override;

    /**
     * @brief method to get id of the session
     * @return id of the session
     */
    SessionId id() const override {
      return 0ull;
    }

   private:
    /**
     * @brief stops session
     */
    void stop();

    /**
     * @brief process http request, compose and execute response
     * @tparam Body request body type
     * @param request request
     */
    void handleRequest(Request &&request);

    /**
     * @brief asynchronously read http message
     */
    void asyncRead();

    /**
     * @brief sends http message
     * @tparam Message http message type
     * @param message http message
     */
    void asyncWrite(Response message);

    /**
     * @brief read completion callback
     */
    void onRead(boost::system::error_code ec, std::size_t size);

    /**
     * @brief write completion callback
     */
    void onWrite(boost::system::error_code ec, std::size_t, bool close);

    /**
     * @brief composes `bad request` message
     * @param message text to send
     * @param version protocol version
     * @param keep_alive true if server should keep connection alive, false
     * otherwise
     * @return composed request
     */
    auto makeBadResponse(std::string_view message,
                         unsigned version,
                         bool keep_alive);

    /**
     * @brief reports error code and message
     * @param ec error code
     * @param message error message
     */
    void reportError(boost::system::error_code ec, std::string_view message);

    static constexpr boost::string_view kServerName = "Kagome";

    /// Strand to ensure the connection's handlers are not called concurrently.
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;

    Configuration config_;              ///< session configuration
    boost::beast::tcp_stream stream_;   ///< stream
    boost::beast::flat_buffer buffer_;  ///< read buffer

    /**
     * @brief request parser type
     */

    std::unique_ptr<Parser> parser_;  ///< http parser
  };
