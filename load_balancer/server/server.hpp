#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <boost/lockfree/queue.hpp>

#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <load_balancer/common.hpp>
#include <load_balancer/server/algorithms/constant.hpp>
#include <load_balancer/server/algorithms/round_robin.hpp>
#include <load_balancer/server/relay.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace eoanermine {

namespace load_balancer {

void do_session(beast::tcp_stream &client, beast::tcp_stream &target,
                net::yield_context yield);
void do_listen(net::io_context &ioc, tcp::endpoint endpoint,
               std::shared_ptr<Algorithm> &algorithm, net::yield_context yield);

class server {
  int threads;
  net::io_context ioc;

public:
  server(int threads = std::thread::hardware_concurrency())
      : threads{threads}, ioc(threads) {}
  void run(std::string_view host, std::string_view port, AlgorithmInfo info,
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
};

} // namespace load_balancer

} // namespace eoanermine
