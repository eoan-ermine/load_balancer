#pragma once

#include <load_balancer/server/algorithms/round_robin.hpp>

namespace eoanermine {

namespace load_balancer {

class StickyRoundRobin : public RoundRobin {
  int stickFactor, stickCounter{0};

public:
  StickyRoundRobin(std::vector<TargetInfo> &targets, std::size_t stickFactor);
  const TargetInfo &getNext() override;
  ~StickyRoundRobin();
};

} // namespace load_balancer

} // namespace eoanermine