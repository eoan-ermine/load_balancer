#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#define ASYNC_WRITE_IMPLEMENTATION(IS_REQUEST)                                 \
  void async_write(SerializerType<IS_REQUEST> &serializer,                     \
                   boost::beast::error_code &ec,                               \
                   boost::asio::yield_context yield) {                         \
    boost::beast::http::async_write(stream, serializer, yield[ec]);            \
  }

#define ASYNC_READ_IMPLEMENTATION(IS_REQUEST)                                  \
  void async_read(                                                             \
      boost::beast::flat_buffer &buffer, MessageType<IS_REQUEST> &message,     \
      boost::beast::error_code &ec, boost::asio::yield_context yield) {        \
    boost::beast::http::async_read(stream, buffer, message, yield[ec]);        \
  }

namespace eoanermine {

namespace load_balancer {

class CommonHTTPTransport {
public:
  template <bool isRequest>
  using SerializerType = boost::beast::http::serializer<
      isRequest, boost::beast::http::dynamic_body, boost::beast::http::fields>;
  template <bool isRequest>
  using MessageType =
      boost::beast::http::message<isRequest, boost::beast::http::dynamic_body>;

  virtual void connect(const TargetInfo &target, boost::beast::error_code &ec,
                       boost::asio::yield_context yield) = 0;

  virtual boost::beast::tcp_stream &get_stream() = 0;

  virtual void async_write(SerializerType<true> &serializer,
                           boost::beast::error_code &ec,
                           boost::asio::yield_context yield) = 0;
  virtual void async_write(SerializerType<false> &serializer,
                           boost::beast::error_code &ec,
                           boost::asio::yield_context yield) = 0;
  virtual void async_read(boost::beast::flat_buffer &buffer,
                          MessageType<true> &message,
                          boost::beast::error_code &ec,
                          boost::asio::yield_context yield) = 0;
  virtual void async_read(boost::beast::flat_buffer &buffer,
                          MessageType<false> &message,
                          boost::beast::error_code &ec,
                          boost::asio::yield_context yield) = 0;

  virtual void disconnect(boost::beast::error_code &ec,
                          boost::asio::yield_context yield) = 0;
  virtual ~CommonHTTPTransport() {}
};

} // namespace load_balancer

} // namespace eoanermine