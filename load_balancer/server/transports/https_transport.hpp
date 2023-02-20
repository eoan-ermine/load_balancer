#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>

#include <load_balancer/server/transports/common_http_transport.hpp>

namespace eoanermine {

namespace load_balancer {

class HTTPSTransport : public CommonHTTPTransport {
  boost::beast::ssl_stream<boost::beast::tcp_stream> stream;

public:
  HTTPSTransport(boost::beast::ssl_stream<boost::beast::tcp_stream> &&stream)
      : stream(std::move(stream)) {}
  void connect(const TargetInfo &target, boost::beast::error_code &ec,
               boost::asio::yield_context yield) override {
    if (!SSL_set_tlsext_host_name(stream.native_handle(),
                                  target.domain.data())) {
      ec.assign(static_cast<int>(::ERR_get_error()),
                boost::asio::error::get_ssl_category());
      std::cerr << ec.message() << "\n";
      return;
    }
    boost::beast::get_lowest_layer(stream).expires_after(
        std::chrono::seconds(30));
    get_lowest_layer(stream).async_connect(target.resolver_results, yield[ec]);
    if (ec)
      return fail(ec, "connect");

    boost::beast::get_lowest_layer(stream).expires_after(
        std::chrono::seconds(30));
    stream.async_handshake(boost::asio::ssl::stream_base::client, yield[ec]);
    if (ec)
      return fail(ec, "handshake");
  }
  void disconnect(boost::beast::error_code &ec,
                  boost::asio::yield_context yield) override {
    stream.async_shutdown(yield[ec]);
    if (ec == boost::asio::error::eof) {
      ec = {};
    }
    if (ec)
      return fail(ec, "shutdown");
  }

  ASYNC_READ_IMPLEMENTATION(false)
  ASYNC_READ_IMPLEMENTATION(true)

  ASYNC_WRITE_IMPLEMENTATION(false)
  ASYNC_WRITE_IMPLEMENTATION(true)

  boost::beast::tcp_stream &get_stream() { return stream.next_layer(); }
};

} // namespace load_balancer

} // namespace eoanermine