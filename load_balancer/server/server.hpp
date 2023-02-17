#pragma once

#include <boost/beast/core.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace beast = boost::beast;
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
  server(int threads = std::thread::hardware_concurrency());
  void run(std::string_view host, std::string_view port, AlgorithmInfo info, std::vector<std::pair<std::string_view, std::string_view>> targets_addrs);
};

} // namespace load_balancer

} // namespace eoanermine
