#include <load_balancer/server/algorithms/sticky_round_robin.hpp>

namespace eoanermine {

namespace load_balancer {

StickyRoundRobbin::StickyRoundRobbin(std::vector<TargetInfo> &targets, std::size_t stickFactor) : RoundRobin(targets) {
  stickFactor = stickFactor;
}

const TargetInfo &StickyRoundRobbin::getNext() {
  static std::size_t idx;
  if (!(stickCounter--)) {
    targetsIdx.pop(idx);
    targetsIdx.push(idx);
    stickCounter = stickFactor;
  }
  return targets[idx];
};

StickyRoundRobbin::~StickyRoundRobbin() {}

} // namespace load_balancer

} // namespace eoanermine