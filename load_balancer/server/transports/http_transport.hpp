#pragma once

#include <boost/beast/core.hpp>

#include <load_balancer/server/transports/transport.hpp>

namespace eoanermine {

namespace load_balancer {

class HTTPTransport : public Transport {
  boost::beast::tcp_stream stream;

public:
  HTTPTransport(boost::beast::tcp_stream &&stream) : stream(std::move(stream)) {}
  void connect(const TargetInfo &target, boost::beast::error_code &ec,
               boost::asio::yield_context yield) override {
    stream.async_connect(target.resolver_results, yield[ec]);
  }
  void async_write_header(SerializerType &serializer,
                          boost::beast::error_code &ec,
                          boost::asio::yield_context yield) override {
    boost::beast::http::async_write_header(stream, serializer, yield[ec]);
  }
  void async_write(SerializerType &serializer, boost::beast::error_code &ec,
                   boost::asio::yield_context yield) override {
    boost::beast::http::async_write(stream, serializer, yield[ec]);
  }
  void async_read(boost::beast::flat_buffer &buffer, ResponseType &response,
                  boost::beast::error_code &ec,
                  boost::asio::yield_context yield) override {
    boost::beast::http::async_read(stream, buffer, response, yield[ec]);
  }
  void disconnect(boost::beast::error_code &ec,
                  boost::asio::yield_context yield) override {
    stream.close();
  }
};

} // namespace load_balancer

} // namespace eoanermine