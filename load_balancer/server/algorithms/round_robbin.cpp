#include <load_balancer/server/algorithms/round_robin.hpp>

namespace eoanermine {

namespace load_balancer {

RoundRobin::RoundRobin(
    std::vector<boost::asio::ip::tcp::resolver::results_type> &targets)
    : targets(targets) {
  for (std::size_t i = 0; i != targets.size(); ++i) {
    targetsIdx.push(i);
  }
}

boost::asio::ip::tcp::resolver::results_type &RoundRobin::getNext() {
  std::size_t idx;
  targetsIdx.pop(idx);
  targetsIdx.push(idx);
  return targets[idx];
};

RoundRobin::~RoundRobin() {}

} // namespace load_balancer

} // namespace eoanermine