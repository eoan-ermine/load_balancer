#include <functional>

#include <load_balancer/server/algorithms/ip_hash.hpp>

namespace eoanermine {

namespace load_balancer {

IPHash::IPHash(const std::vector<TargetInfo> &targets) : targets(targets) {}

const TargetInfo &IPHash::getNext(const RequestInfo &request) {
  std::size_t hash =
      std::hash<unsigned long>{}(request.address.to_v4().to_ulong());
  return targets[hash % targets.size()];
};

IPHash::~IPHash() {}

} // namespace load_balancer

} // namespace eoanermine