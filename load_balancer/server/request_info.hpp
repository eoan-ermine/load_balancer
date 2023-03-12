#pragma once

#include <boost/asio/ip/address.hpp>

namespace eoanermine {

namespace load_balancer {

struct RequestInfo {
  boost::asio::ip::address address;
};

} // namespace load_balancer

} // namespace eoanermine