#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/certify/https_verification.hpp>
#include <boost/config.hpp>
#include <boost/lockfree/queue.hpp>

#include <chrono>

#include <load_balancer/common.hpp>
#include <load_balancer/server/algorithms/constant.hpp>
#include <load_balancer/server/algorithms/round_robin.hpp>
#include <load_balancer/server/relay.hpp>

#include <load_balancer/server/server.hpp>

#include <load_balancer/server/transports/http_transport.hpp>
#include <load_balancer/server/transports/https_transport.hpp>

namespace http = beast::http;

namespace eoanermine {

namespace load_balancer {

void do_session(beast::tcp_stream &client_stream,
                std::shared_ptr<Transport> target_transport,
                TargetInfo &target_info, net::yield_context yield) {
  beast::error_code ec;
  beast::flat_buffer buffer;
  buffer.reserve(2048);

  relay(target_transport, client_stream, target_info, buffer, ec, yield);

  http::response<http::dynamic_body> res;
  target_transport->async_read(buffer, res, ec, yield);
  if (ec)
    return fail(ec, "response read");

  http::async_write(client_stream, res, yield[ec]);
  if (ec)
    return fail(ec, "response write");

  client_stream.socket().shutdown(tcp::socket::shutdown_send, ec);
  target_transport->disconnect(ec, yield);
}

void do_listen(net::io_context &ioc, ssl::context &ctx, tcp::endpoint endpoint,
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

    const TargetInfo &nextTarget = algorithm->getNext();

    std::shared_ptr<Transport> transport;
    if (nextTarget.use_https) {
      transport = std::make_shared<HTTPSTransport>(
          beast::ssl_stream<beast::tcp_stream>(ioc, ctx));
    } else {
      transport = std::make_shared<HTTPTransport>(beast::tcp_stream(ioc));
    }
    transport->connect(nextTarget, ec, yield);

    boost::asio::spawn(
        acceptor.get_executor(),
        std::bind(&do_session, beast::tcp_stream(std::move(client_socket)),
                  transport, std::move(nextTarget), std::placeholders::_1));
  }
}

server::server(int threads) : threads{threads}, ioc(threads) {
  ctx.set_default_verify_paths();
  ctx.set_verify_mode(ssl::verify_peer);
}
void server::run(
    std::string_view host, std::string_view port, AlgorithmInfo info,
    std::vector<std::tuple<std::string_view, std::string_view, bool>>
        targets_addrs) {
  tcp::resolver resolver(ioc);

  std::vector<TargetInfo> targets;
  for (std::size_t i = 0; i != targets_addrs.size(); ++i) {
    auto [domain, port, use_https] = targets_addrs[i];
    targets.push_back(TargetInfo{domain, port, use_https,
                                 std::move(resolver.resolve(domain, port))});
  }

  std::shared_ptr<Algorithm> algorithm;
  switch (info.type) {
  case Algorithm::Type::ROUND_ROBIN:
    algorithm = std::make_shared<RoundRobin>(targets);
    break;
  case Algorithm::Type::CONSTANT:
    algorithm = std::make_shared<Constant>(targets[info.targetIdx]);
    break;
  }

  auto endpoint =
      tcp::endpoint{net::ip::make_address(host),
                    static_cast<unsigned short>(std::atoi(port.data()))};
  boost::asio::spawn(ioc, std::bind(&do_listen, std::ref(ioc), std::ref(ctx),
                                    endpoint, std::ref(algorithm),
                                    std::placeholders::_1));

  std::vector<std::thread> v;
  v.reserve(threads - 1);
  for (auto i = threads - 1; i != 0; --i) {
    v.emplace_back([&] { ioc.run(); });
  }
  ioc.run();
}

} // namespace load_balancer

} // namespace eoanermine
