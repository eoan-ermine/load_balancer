#pragma once

#include <boost/asio/ip/tcp.hpp>

#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <load_balancer/server/algorithms/algorithm.hpp>

namespace eoanermine {

namespace load_balancer {

class server {
  int threads;
  boost::asio::io_context ioc;

public:
  server(int threads = std::thread::hardware_concurrency());
  void run(std::string_view host, std::string_view port, AlgorithmInfo info,
           std::vector<std::tuple<std::string_view, std::string_view, bool>>
               targets_addrs);
};

} // namespace load_balancer

} // namespace eoanermine
