#include <load_balancer/server/algorithms/weighted_round_robin.hpp>

namespace eoanermine {

namespace load_balancer {

WeightedRoundRobin::WeightedRoundRobin(std::vector<TargetInfo> &targets,
                                       std::vector<int> &weights)
    : RoundRobin(targets), weights(weights) {}

const TargetInfo &WeightedRoundRobin::getNext(const RequestInfo &) {
  static std::size_t idx;
  static std::size_t stickCounter = 0;

  if (!(stickCounter--)) {
    targetsIdx.pop(idx);
    targetsIdx.push(idx);
    stickCounter = weights[idx] - 1;
  }

  return targets[idx];
};

WeightedRoundRobin::~WeightedRoundRobin() {}

} // namespace load_balancer

} // namespace eoanermine
