#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace eoanermine {

namespace load_balancer {

class Transport {
public:
  virtual void connect(const TargetInfo &target, boost::beast::error_code &ec,
                       boost::asio::yield_context yield) = 0;
  virtual void async_write_header(
      boost::beast::http::serializer<true, boost::beast::http::buffer_body,
                                     boost::beast::http::fields>
          serializer,
      boost::beast::error_code &ec, boost::asio::yield_context yield) = 0;
  virtual void async_read(
      boost::beast::flat_buffer &buffer,
      boost::beast::http::response<boost::beast::http::dynamic_body> &response,
      boost::beast::error_code &ec, boost::asio::yield_context yield) = 0;
  virtual void disconnect(boost::beast::error_code &ec,
                          boost::asio::yield_context yield) = 0;
  virtual ~Transport() {}
};

} // namespace load_balancer

} // namespace eoanermine