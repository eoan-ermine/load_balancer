#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <boost/variant.hpp>
#include <functional>

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
  virtual boost::variant<std::reference_wrapper<boost::beast::tcp_stream>,
                         std::reference_wrapper<boost::beast::ssl_stream<
                             boost::beast::tcp_stream>>>
  get_original_stream() = 0;

  template <bool isRequest>
  void async_write(SerializerType<isRequest> &serializer,
                   boost::beast::error_code &ec,
                   boost::asio::yield_context yield) {
    auto original_stream = this->get_original_stream();
    if (std::reference_wrapper<boost::beast::tcp_stream> *tcp_stream =
            boost::get<std::reference_wrapper<boost::beast::tcp_stream>>(
                &original_stream)) {
      boost::beast::http::async_write(tcp_stream->get(), serializer, yield[ec]);
    }
    if (std::reference_wrapper<
            boost::beast::ssl_stream<boost::beast::tcp_stream>> *ssl_stream =
            boost::get<std::reference_wrapper<
                boost::beast::ssl_stream<boost::beast::tcp_stream>>>(
                &original_stream)) {
      boost::beast::http::async_write(ssl_stream->get(), serializer, yield[ec]);
    }
  }
  template <bool isRequest>
  void async_read(boost::beast::flat_buffer &buffer,
                  MessageType<isRequest> &message, boost::beast::error_code &ec,
                  boost::asio::yield_context yield) {
    auto original_stream = this->get_original_stream();
    if (std::reference_wrapper<boost::beast::tcp_stream> *tcp_stream =
            boost::get<std::reference_wrapper<boost::beast::tcp_stream>>(
                &original_stream)) {
      boost::beast::http::async_read(tcp_stream->get(), buffer, message,
                                     yield[ec]);
    }
    if (std::reference_wrapper<
            boost::beast::ssl_stream<boost::beast::tcp_stream>> *ssl_stream =
            boost::get<std::reference_wrapper<
                boost::beast::ssl_stream<boost::beast::tcp_stream>>>(
                &original_stream)) {
      boost::beast::http::async_read(ssl_stream->get(), buffer, message,
                                     yield[ec]);
    }
  }

  virtual void disconnect(boost::beast::error_code &ec,
                          boost::asio::yield_context yield) = 0;
  virtual ~CommonHTTPTransport() {}
};

} // namespace load_balancer

} // namespace eoanermine