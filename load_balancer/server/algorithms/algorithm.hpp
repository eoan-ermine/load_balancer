#pragma once

#include <iostream>
#include <vector>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>

namespace eoanermine {

namespace load_balancer {

class Algorithm {
public:
  enum class Type { ROUND_ROBIN, CONSTANT };
  virtual boost::asio::ip::tcp::resolver::results_type &getNext() = 0;
  virtual ~Algorithm() {}
};

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

struct AlgorithmInfo {
  Algorithm::Type type;
  std::size_t targetIdx;
};

} // namespace load_balancer

} // namespace eoanermine