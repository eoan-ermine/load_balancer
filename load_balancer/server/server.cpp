#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <boost/lockfree/queue.hpp>

#include <load_balancer/common.hpp>
#include <load_balancer/server/algorithms/constant.hpp>
#include <load_balancer/server/algorithms/round_robin.hpp>
#include <load_balancer/server/relay.hpp>

#include <load_balancer/server/server.hpp>

namespace http = beast::http;

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

server::server(int threads)
    : threads{threads}, ioc(threads) {}
void server::run(std::string_view host, std::string_view port, AlgorithmInfo info,
         std::vector<std::pair<std::string_view, std::string_view>>
             targets_addrs) {
  tcp::resolver resolver(ioc);

  std::vector<tcp::resolver::results_type> targets;
  for (std::size_t i = 0; i != targets_addrs.size(); ++i) {
    targets.push_back(
        resolver.resolve(targets_addrs[i].first, targets_addrs[i].second));
  }

  std::shared_ptr<Algorithm> algorithm;
  switch (info.type) {
  case Algorithm::Type::ROUND_ROBIN:
    algorithm = std::make_shared<RoundRobin>(targets);
    break;
  case Algorithm::Type::CONSTANT:
    algorithm = std::make_shared<Constant>(targets, info.targetIdx);
    break;
  }

  auto endpoint =
      tcp::endpoint{net::ip::make_address(host),
                    static_cast<unsigned short>(std::atoi(port.data()))};
  boost::asio::spawn(ioc,
                     std::bind(&do_listen, std::ref(ioc), endpoint,
                               std::ref(algorithm), std::placeholders::_1));

  std::vector<std::thread> v;
  v.reserve(threads - 1);
  for (auto i = threads - 1; i != 0; --i) {
    v.emplace_back([&] { ioc.run(); });
  }
  ioc.run();
}

} // namespace load_balancer

} // namespace eoanermine
