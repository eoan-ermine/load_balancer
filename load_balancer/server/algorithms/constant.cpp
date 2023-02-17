#include <load_balancer/server/algorithms/constant.hpp>

namespace eoanermine {

namespace load_balancer {

Constant::Constant(std::vector<boost::asio::ip::tcp::resolver::results_type> &targets, std::size_t targetIdx)
  : target(targets[targetIdx]) {}

boost::asio::ip::tcp::resolver::results_type &Constant::getNext() {
	return target;
};

Constant::~Constant() {}

} // namespace load_balancer

} // namespace eoanermine