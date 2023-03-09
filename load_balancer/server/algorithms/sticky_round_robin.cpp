#include <load_balancer/server/algorithms/sticky_round_robin.hpp>

namespace eoanermine {

namespace load_balancer {

StickyRoundRobbin::StickyRoundRobbin(std::vector<TargetInfo> &targets, int stickFactor) : RoundRobin(targets) {
  stickCounter = stickFactor = stickFactor;
}

const TargetInfo &StickyRoundRobbin::getNext() {
  static std::size_t idx;
  if (!(stickCounter--)) {
    targetsIdx.pop(idx);
    targetsIdx.push(idx);
  }
  return targets[idx];
};

StickyRoundRobbin::~StickyRoundRobbin() {}

} // namespace load_balancer

} // namespace eoanermine