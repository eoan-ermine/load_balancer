#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>

#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <load_balancer/server/target_info.hpp>
#include <load_balancer/server/transports/transport.hpp>

namespace beast = boost::beast;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

namespace eoanermine {

namespace load_balancer {

void do_session(beast::tcp_stream &client_stream,
                std::shared_ptr<Transport> target_transport,
                TargetInfo &target_info, net::yield_context yield);
void do_listen(net::io_context &ioc, ssl::context &ctx, tcp::endpoint endpoint,
               std::shared_ptr<Algorithm> &algorithm, net::yield_context yield);

class server {
  int threads;
  net::io_context ioc;
  ssl::context ctx{ssl::context::tlsv12_client};

public:
  server(int threads = std::thread::hardware_concurrency());
  void run(std::string_view host, std::string_view port, AlgorithmInfo info,
           std::vector<std::tuple<std::string_view, std::string_view, unsigned>>
               targets_addrs);
};

} // namespace load_balancer

} // namespace eoanermine
