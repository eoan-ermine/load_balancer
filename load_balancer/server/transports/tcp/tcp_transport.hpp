#pragma once

#include <load_balancer/server/transports/tcp/common_tcp_transport.hpp>

namespace eoanermine {

namespace load_balancer {

class TCPTransport : CommonTCPTransport {
  tcp::socket socket;

public:
  TCPTransport(tcp::socket &&socket): socket(socket) {}
  void connect(tcp::endpoint endpoint, boost::asio::error_code &ec,
               boost::asio::yield_context yield) {
    socket.async_connect(
      endpoint, yield[ec]
    );
    if (ec)
      return fail("async_connect");
  }

  void async_read(boost::beast::flat_buffer &buffer, boost::beast::error_code &ec,
                  boost::asio::yield_context yield) {
    socket.async_receive(
      buffer, yield[ec]
    );
    if (ec)
        return fail("async_receive")
  }
  void async_write(boost::beast::flat_buffer &buffer, boost::beast::error_code &ec,
                           boost::asio::yield_context yield) {
    socket.async_send(
      buffer, yield[ec]
    );
    if (ec)
      return fail("async_send")
  }

  void disconnect(boost::asio::error_code &ec,
                  boost::asio::yield_context yield) {
    socket.shutdown(
      tcp::socket::shutdown_type::both, yield[ec]
    );
    if (ec)
      return fail("shutdown")
  }
  ~TCPTransport() {}
};

} // namespace load_balancer

} // namespace eoanermine
