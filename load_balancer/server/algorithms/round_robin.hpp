#pragma once

#include <boost/lockfree/queue.hpp>
#include <vector>

#include <load_balancer/server/algorithms/algorithm.hpp>

namespace eoanermine {

namespace load_balancer {

class RoundRobin : public Algorithm {
private:
  std::vector<boost::asio::ip::tcp::resolver::results_type> &targets;
  boost::lockfree::queue<std::size_t, boost::lockfree::capacity<64>> targetsIdx;

public:
  RoundRobin(
      std::vector<boost::asio::ip::tcp::resolver::results_type> &targets);
  boost::asio::ip::tcp::resolver::results_type &getNext() override;
  ~RoundRobin();
};

} // namespace load_balancer

} // namespace eoanermine