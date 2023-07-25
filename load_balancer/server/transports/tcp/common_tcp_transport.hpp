#pragma once

#include <boost/asio.hpp>

namespace eoanermine {

namespace load_balancer {

class CommonTCPTransport {
public:
  virtual void connect(tcp::endpoint endpoint, boost::asio::error_code &ec,
                       boost::asio::yield_context yield) = 0;

  virtual void async_read(boost::beast::flat_buffer &buffer, boost::beast::error_code &ec,
  						  boost::asio::yield_context yield) = 0;
  virtual void async_write(boost::beast::flat_buffer &buffer, boost::beast::error_code &ec,
  						   boost::asio::yield_context yield) = 0;

  virtual void disconnect(boost::asio::error_code &ec,
                          boost::asio::yield_context yield) = 0;
  virtual ~CommonTCPTransport() {}
};

} // namespace load_balancer

} // namespace eoanermine
