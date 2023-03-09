#include <load_balancer/server/algorithms/sticky_round_robin.hpp>

namespace eoanermine {

namespace load_balancer {

StickyRoundRobin::StickyRoundRobin(std::vector<TargetInfo> &targets, std::size_t stickFactor) : RoundRobin(targets) {
  stickFactor = stickFactor;
}

const TargetInfo &StickyRoundRobin::getNext() {
  static std::size_t idx;
  if (!(stickCounter--)) {
    targetsIdx.pop(idx);
    targetsIdx.push(idx);
    stickCounter = stickFactor;
  }
  return targets[idx];
};

StickyRoundRobin::~StickyRoundRobin() {}

} // namespace load_balancer

} // namespace eoanermine