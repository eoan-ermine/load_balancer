#include <load_balancer/server/algorithms/weighted_round_robin.hpp>

namespace eoanermine {

namespace load_balancer {

WeightedRoundRobin::WeightedRoundRobin(std::vector<TargetInfo> &targets, std::vector<int>& weights)
  : RoundRobin(targets), weights(weights) { }

const TargetInfo &WeightedRoundRobin::getNext() {
  static std::size_t idx, weightIdx = 0;
  static std::size_t stickCounter = 0;

  if (!(stickCounter--)) {
    targetsIdx.pop(idx);
    targetsIdx.push(idx);

    stickCounter = weights[weightIdx] - 1;
    weightIdx = (weightIdx + 1) % weights.size();
  }

  return targets[idx];
};

WeightedRoundRobin::~WeightedRoundRobin() {}

} // namespace load_balancer

} // namespace eoanermine
