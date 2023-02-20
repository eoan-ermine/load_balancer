#pragma once

#include <boost/asio/spawn.hpp>

#include <memory>

#include <load_balancer/common.hpp>
#include <load_balancer/server/transports/common_http_transport.hpp>

namespace eoanermine {

namespace load_balancer {

template <bool isRequest, class DynamicBuffer,
          class Transform =
              decltype([](CommonHTTPTransport::MessageType<isRequest> &) {})>
void relay(std::shared_ptr<CommonHTTPTransport> &input_transport,
           std::shared_ptr<CommonHTTPTransport> &output_transport,
           DynamicBuffer &buffer, boost::beast::error_code &ec,
           boost::asio::yield_context yield,
           const Transform &transform = Transform()) {
  CommonHTTPTransport::MessageType<isRequest> message;
  input_transport->async_read(buffer, message, ec, yield);
  if (ec)
    return fail(ec, "message read");

  transform(message);

  CommonHTTPTransport::SerializerType<isRequest> serializer{message};
  output_transport->async_write(serializer, ec, yield);
  if (ec)
    return fail(ec, "message write");
}

} // namespace load_balancer

} // namespace eoanermine