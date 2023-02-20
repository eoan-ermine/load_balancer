#include <boost/asio/spawn.hpp>
#include <boost/beast/http.hpp>

#include <chrono>

#include <load_balancer/common.hpp>
#include <load_balancer/server/algorithms/constant.hpp>
#include <load_balancer/server/algorithms/round_robin.hpp>

#include <load_balancer/server/extensions/http_extension.hpp>
#include <load_balancer/server/server.hpp>

namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

namespace eoanermine {

namespace load_balancer {

server::server(int threads) : threads{threads}, ioc(threads) {}
void server::run(
    std::string_view host, std::string_view port, AlgorithmInfo info,
    std::vector<std::tuple<std::string_view, std::string_view, bool>>
        targets_addrs) {
  tcp::resolver resolver(ioc);

  std::vector<TargetInfo> targets;
  for (std::size_t i = 0; i != targets_addrs.size(); ++i) {
    auto [domain, port, use_https] = targets_addrs[i];
    targets.push_back(TargetInfo{domain, port, use_https,
                                 std::move(resolver.resolve(domain, port))});
  }

  std::shared_ptr<Algorithm> algorithm;
  switch (info.type) {
  case Algorithm::Type::ROUND_ROBIN:
    algorithm = std::make_shared<RoundRobin>(targets);
    break;
  case Algorithm::Type::CONSTANT:
    algorithm = std::make_shared<Constant>(targets[info.targetIdx]);
    break;
  }

  HTTPExtension extension(ioc);
  auto endpoint =
      tcp::endpoint{net::ip::make_address(host),
                    static_cast<unsigned short>(std::atoi(port.data()))};
  net::spawn(ioc, std::bind(&HTTPExtension::listen, &extension, endpoint,
                            std::ref(algorithm), std::placeholders::_1));

  std::vector<std::thread> v;
  v.reserve(threads - 1);
  for (auto i = threads - 1; i != 0; --i) {
    v.emplace_back([&] { ioc.run(); });
  }
  ioc.run();
}

} // namespace load_balancer

} // namespace eoanermine
