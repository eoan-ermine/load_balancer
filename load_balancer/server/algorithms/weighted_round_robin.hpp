#pragma once

#include <load_balancer/server/algorithms/round_robin.hpp>

namespace eoanermine {

namespace load_balancer {

class WeightedRoundRobin : public RoundRobin {
  std::vector<float> &weights;

public:
  WeightedRoundRobin(std::vector<TargetInfo> &targets, std::vector<int>& weights);
  const TargetInfo &getNext() override;
  ~WeightedRoundRobin();
};

} // namespace load_balancer

} // namespace eoanermine