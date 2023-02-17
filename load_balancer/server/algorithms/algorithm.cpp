#include <load_balancer/server/algorithms/algorithm.hpp>

namespace eoanermine {

namespace load_balancer {

std::istream &operator>>(std::istream &in, Algorithm::Type &algorithm) {
  std::string token;
  in >> token;
  if (token == "round_robin")
    algorithm = Algorithm::Type::ROUND_ROBIN;
  else if (token == "constant")
    algorithm = Algorithm::Type::CONSTANT;
  return in;
}

std::ostream &operator<<(std::ostream &out, const Algorithm::Type &algorithm) {
  if (algorithm == Algorithm::Type::ROUND_ROBIN)
    out << "round_robin";
  else if (algorithm == Algorithm::Type::CONSTANT)
    out << "constant";
  return out;
}

Algorithm::~Algorithm() {}

} // namespace load_balancer

} // namespace eoanermine