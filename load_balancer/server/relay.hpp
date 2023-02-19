#pragma once

#include <boost/asio/spawn.hpp>
#include <boost/beast/core/make_printable.hpp>

#include <memory>

#include <load_balancer/common.hpp>
#include <load_balancer/server/target_info.hpp>
#include <load_balancer/server/transports/transport.hpp>

namespace eoanermine {

namespace load_balancer {

template <class AsyncReadStream, class DynamicBuffer>
void relay(std::shared_ptr<Transport> &output_transport, AsyncReadStream &input,
           TargetInfo &target_info, DynamicBuffer &buffer,
           boost::beast::error_code &ec, boost::asio::yield_context yield) {
  boost::beast::http::request<boost::beast::http::dynamic_body> request;
  boost::beast::http::async_read(input, buffer, request, yield[ec]);
  if (ec)
    return fail(ec, "request read");

  request.set("Host", boost::string_view{target_info.domain.data(),
                                         target_info.domain.size()});

  Transport::SerializerType serializer{request};
  output_transport->async_write(serializer, ec, yield);
  if (ec)
    return fail(ec, "request write");
}

} // namespace load_balancer

} // namespace eoanermine