#pragma once

#include <vector>

#include <load_balancer/server/algorithms/algorithm.hpp>

namespace eoanermine {

namespace load_balancer {

class IPHash : public Algorithm {
  std::vector<TargetInfo> &targets;

public:
  IPHash(std::vector<TargetInfo> &targets);
  const TargetInfo &getNext(const RequestInfo &request) override;
  ~IPHash();
};

} // namespace load_balancer

} // namespace eoanermine