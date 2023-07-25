#pragma once

#include <boost/asio/spawn.hpp>

#include <memory>

#include <load_balancer/common.hpp>
#include <load_balancer/server/transports/common_http_transport.hpp>

namespace eoanermine {

namespace load_balancer {

template <bool isRequest, class DynamicBuffer>
void relay(std::shared_ptr<CommonTCPTransport> &input_transport,
           std::shared_ptr<CommonTCPTransport> &output_transport,
           DynamicBuffer &buffer, boost::beast::error_code &ec,
           boost::asio::yield_context yield) {
  input_transport->async_read(buffer, ec, yield);
  if (ec)
    return fail(ec, "buffer read");

  output_transport->async_write(buffer, ec, yield);
  if (ec)
    return fail(ec, "buffer write");
}

} // namespace load_balancer

} // namespace eoanermine