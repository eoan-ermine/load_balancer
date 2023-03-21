#include <load_balancer/server/algorithms/constant.hpp>

namespace eoanermine {

namespace load_balancer {

Constant::Constant(const TargetInfo &target) : target(target) {}

const TargetInfo &Constant::getNext(const RequestInfo &) { return target; };

Constant::~Constant() {}

} // namespace load_balancer

} // namespace eoanermine