#include <load_balancer/server/algorithms/constant.hpp>

namespace eoanermine {

namespace load_balancer {

Constant::Constant(boost::asio::ip::tcp::resolver::results_type &target)
    : target(target) {}

boost::asio::ip::tcp::resolver::results_type &Constant::getNext() {
  return target;
};

Constant::~Constant() {}

} // namespace load_balancer

} // namespace eoanermine