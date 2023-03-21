#include <load_balancer/server/algorithms/sticky_round_robin.hpp>

namespace eoanermine {

namespace load_balancer {

StickyRoundRobin::StickyRoundRobin(const std::vector<TargetInfo> &targets,
                                   std::size_t stickFactor)
    : RoundRobin(targets) {
  this->stickFactor = stickFactor - 1;
}

const TargetInfo &StickyRoundRobin::getNext(const RequestInfo &) {
  static std::size_t idx;
  static std::size_t stickCounter = 0;

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
