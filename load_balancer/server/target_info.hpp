#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <string_view>

namespace eoanermine {

namespace load_balancer {

struct TargetInfo {
  std::string_view domain;
  std::string_view port;
  bool use_https;
  boost::asio::ip::tcp::resolver::results_type resolver_results;
};

} // namespace load_balancer

} // namespace eoanermine