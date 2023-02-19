#pragma once

#include <boost/asio/spawn.hpp>

#include <load_balancer/common.hpp>
#include <load_balancer/server/target_info.hpp>

namespace eoanermine {

namespace load_balancer {

template <bool isRequest, class AsyncWriteStream, class AsyncReadStream,
          class DynamicBuffer>
void relay(AsyncWriteStream &output, AsyncReadStream &input,
           TargetInfo &target_info, DynamicBuffer &buffer,
           boost::beast::error_code &ec, boost::asio::yield_context yield) {
  static_assert(boost::beast::is_async_write_stream<AsyncWriteStream>::value,
                "AsyncWriteStream requirements not met");

  static_assert(boost::beast::is_async_read_stream<AsyncReadStream>::value,
                "AsyncReadStream requirements not met");

  boost::beast::http::parser<isRequest, boost::beast::http::buffer_body> p;
  boost::beast::http::async_read_header(input, buffer, p, yield[ec]);
  if (ec)
    return fail(ec, "header read");

  p.get().set("Host", boost::string_view{target_info.domain.data(),
                                         target_info.domain.size()});

  boost::beast::http::serializer<isRequest, boost::beast::http::buffer_body,
                                 boost::beast::http::fields>
      sr{p.get()};
  boost::beast::http::async_write_header(output, sr, yield[ec]);
  if (ec)
    return fail(ec, "header write");
}

} // namespace load_balancer

} // namespace eoanermine