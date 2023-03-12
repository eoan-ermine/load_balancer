#pragma once

#include <string>
#include <vector>

#include <load_balancer/server/algorithms/algorithm.hpp>

namespace eoanermine {

namespace load_balancer {

struct CLIArguments {
  /* Required arguments */
  std::vector<std::string> target_hosts, target_ports;

  /* Required arguments with default values */
  std::string listener_host{"127.0.0.1"}, listener_port{"80"};
  Algorithm::Type balancing_algorithm{Algorithm::Type::ROUND_ROBIN};

  /* Optional arguments */
  std::size_t
      target_idx; // Must be specified if `balancing_algorithm` is `constant`
  std::size_t
      stick_factor; // Must be specified if `balancing_algorithm` is `sticky_round_robin`
  std::vector<int>
      weights; // Must be specified if `balancing_algorithm` is `weighted_round_robin`
};

} // namespace load_balancer

} // namespace eoanermine