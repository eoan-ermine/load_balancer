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

  ASYNC_READ_IMPLEMENTATION(false)
  ASYNC_READ_IMPLEMENTATION(true)

  ASYNC_WRITE_IMPLEMENTATION(false)
  ASYNC_WRITE_IMPLEMENTATION(true)

  boost::beast::tcp_stream &get_stream() { return stream; }
};

} // namespace load_balancer

} // namespace eoanermine