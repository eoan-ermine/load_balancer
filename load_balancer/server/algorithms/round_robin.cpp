#include <load_balancer/server/algorithms/round_robin.hpp>

namespace eoanermine {

namespace load_balancer {

RoundRobin::RoundRobin(const std::vector<TargetInfo> &targets)
    : targets(targets) {
  for (std::size_t i = 0; i != targets.size(); ++i) {
    targetsIdx.push(i);
  }
}

const TargetInfo &RoundRobin::getNext(const RequestInfo &) {
  std::size_t idx;
  targetsIdx.pop(idx);
  targetsIdx.push(idx);
  return targets[idx];
};

RoundRobin::~RoundRobin() {}

} // namespace load_balancer

} // namespace eoanermine