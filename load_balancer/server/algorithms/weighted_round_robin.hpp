#pragma once

#include <load_balancer/server/algorithms/round_robin.hpp>

namespace eoanermine {

namespace load_balancer {

class WeightedRoundRobin : public RoundRobin {
  const std::vector<int> &weights;

public:
  WeightedRoundRobin(const std::vector<TargetInfo> &targets,
                     const std::vector<int> &weights);
  const TargetInfo &getNext(const RequestInfo &) override;
  ~WeightedRoundRobin();
};

} // namespace load_balancer

} // namespace eoanermine