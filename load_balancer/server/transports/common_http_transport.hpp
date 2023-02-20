#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace eoanermine {

namespace load_balancer {

class CommonHTTPTransport {
public:
  using SerializerType =
      boost::beast::http::serializer<true, boost::beast::http::dynamic_body,
                                     boost::beast::http::fields>;
  using ResponseType =
      boost::beast::http::response<boost::beast::http::dynamic_body>;

  virtual void connect(const TargetInfo &target, boost::beast::error_code &ec,
                       boost::asio::yield_context yield) = 0;
  virtual void async_write(SerializerType &serializer,
                           boost::beast::error_code &ec,
                           boost::asio::yield_context yield) = 0;
  virtual void async_read(boost::beast::flat_buffer &buffer,
                          ResponseType &response, boost::beast::error_code &ec,
                          boost::asio::yield_context yield) = 0;
  virtual void disconnect(boost::beast::error_code &ec,
                          boost::asio::yield_context yield) = 0;
  virtual ~CommonHTTPTransport() {}
};

} // namespace load_balancer

} // namespace eoanermine