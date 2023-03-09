#pragma once

#include <load_balancer/server/algorithms/round_robin.hpp>

namespace eoanermine {

namespace load_balancer {

class StickyRoundRobbin : public RoundRobin {
  int stickFactor, stickCounter;

public:
  StickyRoundRobbin(std::vector<TargetInfo> &targets, int stickFactor);
  const TargetInfo &getNext() override;
  ~StickyRoundRobbin();
};

} // namespace load_balancer

} // namespace eoanermine