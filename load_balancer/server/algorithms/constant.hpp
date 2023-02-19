#pragma once

#include <load_balancer/server/algorithms/algorithm.hpp>

namespace eoanermine {

namespace load_balancer {

class Constant : public Algorithm {
private:
  boost::asio::ip::tcp::resolver::results_type &target;

public:
  Constant(boost::asio::ip::tcp::resolver::results_type &target);
  boost::asio::ip::tcp::resolver::results_type &getNext() override;
  ~Constant();
};

} // namespace load_balancer

} // namespace eoanermine