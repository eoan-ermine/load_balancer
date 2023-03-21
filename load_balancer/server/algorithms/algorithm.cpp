#include <load_balancer/server/algorithms/algorithm.hpp>

#include <load_balancer/server/algorithms/constant.hpp>
#include <load_balancer/server/algorithms/ip_hash.hpp>
#include <load_balancer/server/algorithms/round_robin.hpp>
#include <load_balancer/server/algorithms/sticky_round_robin.hpp>
#include <load_balancer/server/algorithms/weighted_round_robin.hpp>

namespace eoanermine {

namespace load_balancer {

std::istream &operator>>(std::istream &in, Algorithm::Type &algorithm) {
  std::string token;
  in >> token;
  if (token == "constant")
    algorithm = Algorithm::Type::CONSTANT;
  else if (token == "round_robin")
    algorithm = Algorithm::Type::ROUND_ROBIN;
  else if (token == "sticky_round_robin")
    algorithm = Algorithm::Type::STICKY_ROUND_ROBIN;
  else if (token == "weighted_round_robin")
    algorithm = Algorithm::Type::WEIGHTED_ROUND_ROBIN;
  else if (token == "ip_hash")
    algorithm = Algorithm::Type::IP_HASH;
  return in;
}

std::ostream &operator<<(std::ostream &out, const Algorithm::Type &algorithm) {
  if (algorithm == Algorithm::Type::CONSTANT)
    out << "constant";
  else if (algorithm == Algorithm::Type::ROUND_ROBIN)
    out << "round_robin";
  else if (algorithm == Algorithm::Type::STICKY_ROUND_ROBIN)
    out << "sticky_round_robin";
  else if (algorithm == Algorithm::Type::WEIGHTED_ROUND_ROBIN)
    out << "weighted_round_robin";
  else if (algorithm == Algorithm::Type::IP_HASH)
    out << "ip_hash";
  return out;
}

Algorithm::~Algorithm() {}

std::shared_ptr<Algorithm>
Algorithm::create(const AlgorithmInfo &info,
                  const std::vector<TargetInfo> &targets) {
  switch (info.type) {
  case Algorithm::Type::CONSTANT:
    return std::make_shared<Constant>(targets[info.targetIdx]);
  case Algorithm::Type::ROUND_ROBIN:
    return std::make_shared<RoundRobin>(targets);
  case Algorithm::Type::STICKY_ROUND_ROBIN:
    return std::make_shared<StickyRoundRobin>(targets, info.stickFactor);
  case Algorithm::Type::WEIGHTED_ROUND_ROBIN:
    return std::make_shared<WeightedRoundRobin>(targets, info.weights);
  case Algorithm::Type::IP_HASH:
    return std::make_shared<IPHash>(targets);
  }
}

} // namespace load_balancer

} // namespace eoanermine