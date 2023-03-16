#pragma once

#include <boost/lockfree/queue.hpp>
#include <vector>

#include <load_balancer/server/algorithms/algorithm.hpp>

namespace eoanermine {

namespace load_balancer {

class RoundRobin : public Algorithm {
protected:
  std::vector<TargetInfo> &targets;
  boost::lockfree::queue<std::size_t, boost::lockfree::capacity<64>> targetsIdx;

public:
  RoundRobin(std::vector<TargetInfo> &targets);
  const TargetInfo &getNext(const RequestInfo &) override;
  ~RoundRobin();
};

} // namespace load_balancer

} // namespace eoanermine