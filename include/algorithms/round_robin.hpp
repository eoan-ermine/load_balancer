#pragma once

#include "algorithms/algorithm.hpp"

#include <boost/lockfree/queue.hpp>

namespace eoanermine {

namespace load_balancer {

class RoundRobin: public Algorithm {
private:
	std::vector<boost::asio::ip::tcp::resolver::results_type>& targets;
	boost::lockfree::queue<std::size_t, boost::lockfree::capacity<64>> targetsIdx;
public:
	RoundRobin(std::vector<boost::asio::ip::tcp::resolver::results_type>& targets): targets(targets) {
        for (std::size_t i = 0; i != targets.size(); ++i) {
            targetsIdx.push(i);
        }
	}
	boost::asio::ip::tcp::resolver::results_type& getNext() override {
		std::size_t idx; targetsIdx.pop(idx);
		targetsIdx.push(idx);
		return targets[idx];
	}; 
	~RoundRobin() { }
};

} // namespace load_balancer

} // namespace eoanermine