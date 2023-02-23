#pragma once

#include <boost/beast/core.hpp>

#include <load_balancer/server/transports/common_http_transport.hpp>

namespace eoanermine {

namespace load_balancer {

class HTTPTransport : public CommonHTTPTransport {
  boost::beast::tcp_stream stream;

public:
  HTTPTransport(boost::beast::tcp_stream &&stream)
      : stream(std::move(stream)) {}
  void connect(const TargetInfo &target, boost::beast::error_code &ec,
               boost::asio::yield_context yield) override {
    stream.async_connect(target.resolver_results, yield[ec]);
  }
  void disconnect(boost::beast::error_code &ec,
                  boost::asio::yield_context yield) override {
    stream.close();
  }

  boost::beast::tcp_stream &get_stream() { return stream; }
  boost::variant<std::reference_wrapper<boost::beast::tcp_stream>, std::reference_wrapper<boost::beast::ssl_stream<boost::beast::tcp_stream>>> get_original_stream() {
    return std::reference_wrapper(stream);
  }
};

} // namespace load_balancer

} // namespace eoanermine