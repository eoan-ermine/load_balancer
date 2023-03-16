#pragma once

#include <iosfwd>
#include <vector>

#include <load_balancer/server/request_info.hpp>
#include <load_balancer/server/target_info.hpp>

namespace eoanermine {

namespace load_balancer {

class Algorithm {
public:
  enum class Type {
    CONSTANT,
    ROUND_ROBIN,
    STICKY_ROUND_ROBIN,
    WEIGHTED_ROUND_ROBIN,
    IP_HASH
  };
  virtual const TargetInfo &getNext(const RequestInfo &request) = 0;
  virtual ~Algorithm();
};

std::istream &operator>>(std::istream &in, Algorithm::Type &algorithm);
std::ostream &operator<<(std::ostream &out, const Algorithm::Type &algorithm);

struct AlgorithmInfo {
  Algorithm::Type type;
  std::size_t targetIdx;
  std::size_t stickFactor;
  std::vector<int> weights;
};

} // namespace load_balancer

} // namespace eoanermine