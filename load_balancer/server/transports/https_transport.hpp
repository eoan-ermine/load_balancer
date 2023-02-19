#pragma once

#include <load_balancer/server/transports/transport.hpp>

namespace eoanermine {

namespace load_balancer {

class HTTPSTransport : public Transport {
  beast::ssl_stream<beast::tcp_stream> stream;

public:
  HTTPSTransport(beast::ssl_stream<beast::tcp_stream> &&stream)
      : stream(std::move(stream)) {}
  void connect(const TargetInfo &target, boost::beast::error_code &ec,
               boost::asio::yield_context yield) override {
    if (!SSL_set_tlsext_host_name(stream.native_handle(),
                                  target.domain.data())) {
      ec.assign(static_cast<int>(::ERR_get_error()),
                net::error::get_ssl_category());
      std::cerr << ec.message() << "\n";
      return;
    }
    beast::get_lowest_layer(stream).expires_after(std::chrono::seconds(30));
    get_lowest_layer(stream).async_connect(target.resolver_results, yield[ec]);
    if (ec)
      return fail(ec, "connect");

    beast::get_lowest_layer(stream).expires_after(std::chrono::seconds(30));
    stream.async_handshake(ssl::stream_base::client, yield[ec]);
    if (ec)
      return fail(ec, "handshake");
  }
  void async_write_header(
      boost::beast::http::serializer<true, boost::beast::http::buffer_body,
                                     boost::beast::http::fields>
          serializer,
      boost::beast::error_code &ec, boost::asio::yield_context yield) override {
    boost::beast::http::async_write_header(stream, serializer, yield[ec]);
  }
  void async_read(
      boost::beast::flat_buffer &buffer,
      boost::beast::http::response<boost::beast::http::dynamic_body> &response,
      boost::beast::error_code &ec, boost::asio::yield_context yield) override {
    boost::beast::http::async_read(stream, buffer, response, yield[ec]);
  }
  void disconnect(boost::beast::error_code &ec,
                  boost::asio::yield_context yield) override {
    stream.async_shutdown(yield[ec]);
    if (ec == net::error::eof) {
      ec = {};
    }
    if (ec)
      return fail(ec, "shutdown");
  }
};

} // namespace load_balancer

} // namespace eoanermine