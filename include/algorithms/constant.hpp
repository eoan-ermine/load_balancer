#pragma once

#include "algorithms/algorithm.hpp"

namespace eoanermine {

namespace load_balancer {

class Constant: public Algorithm {
private:
	boost::asio::ip::tcp::resolver::results_type& target;
public:
	Constant(std::vector<boost::asio::ip::tcp::resolver::results_type>& targets, std::size_t targetIdx): target(targets[targetIdx]) { }
	boost::asio::ip::tcp::resolver::results_type& getNext() override {
		return target;
	};
	~Constant() { }
};

} // namespace load_balancer

} // namespace eoanermine