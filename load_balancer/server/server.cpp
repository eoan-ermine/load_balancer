#include <load_balancer/server/server.hpp>

namespace eoanermine {

namespace load_balancer {

void do_session(beast::tcp_stream &client, beast::tcp_stream &target,
                net::yield_context yield) {
  beast::error_code ec;
  beast::flat_buffer buffer;
  buffer.reserve(2048);

  relay<true>(target, client, buffer, ec, yield);

  http::response<http::dynamic_body> res;
  http::async_read(target, buffer, res, yield[ec]);
  if (ec)
    return fail(ec, "response read");

  http::async_write(client, res, yield[ec]);
  if (ec)
    return fail(ec, "response write");

  client.socket().shutdown(tcp::socket::shutdown_send, ec);
  target.close();
}

void do_listen(net::io_context &ioc, tcp::endpoint endpoint,
               std::shared_ptr<Algorithm> &algorithm,
               net::yield_context yield) {
  beast::error_code ec;

  tcp::acceptor acceptor{ioc};
  acceptor.open(endpoint.protocol(), ec);
  if (ec)
    return fail(ec, "open");

  acceptor.set_option(net::socket_base::reuse_address(true), ec);
  if (ec)
    return fail(ec, "set_option");

  acceptor.bind(endpoint, ec);
  if (ec)
    return fail(ec, "bind");

  acceptor.listen(net::socket_base::max_listen_connections, ec);
  if (ec)
    return fail(ec, "listen");

  while (true) {
    tcp::socket client_socket{ioc};
    acceptor.async_accept(client_socket, yield[ec]);
    if (ec)
      return fail(ec, "accept");

    beast::tcp_stream target_stream{ioc};
    target_stream.async_connect(algorithm->getNext(), yield[ec]);
    boost::asio::spawn(
        acceptor.get_executor(),
        std::bind(&do_session, beast::tcp_stream(std::move(client_socket)),
                  std::move(target_stream), std::placeholders::_1));
  }
}

} // namespace load_balancer

} // namespace eoanermine
