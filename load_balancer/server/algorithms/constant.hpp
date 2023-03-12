#pragma once

#include <load_balancer/server/algorithms/algorithm.hpp>

namespace eoanermine {

namespace load_balancer {

class Constant : public Algorithm {
private:
  TargetInfo &target;

public:
  Constant(TargetInfo &target);
  const TargetInfo &getNext(const RequestInfo&) override;
  ~Constant();
};

} // namespace load_balancer

} // namespace eoanermine