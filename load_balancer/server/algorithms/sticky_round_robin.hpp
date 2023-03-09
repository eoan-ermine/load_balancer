#pragma once

#include <load_balancer/server/algorithms/round_robin.hpp>

namespace eoanermine {

namespace load_balancer {

class StickyRoundRobbin : public RoundRobin {
  int stickFactor, stickCounter{0};

public:
  StickyRoundRobbin(std::vector<TargetInfo> &targets, std::size_t stickFactor);
  const TargetInfo &getNext() override;
  ~StickyRoundRobbin();
};

} // namespace load_balancer

} // namespace eoanermine