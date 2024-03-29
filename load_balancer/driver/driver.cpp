#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <load_balancer/driver/driver.hpp>
#include <load_balancer/server/server.hpp>
#include <load_balancer/server/target_info.hpp>

namespace eoanermine {

namespace load_balancer {

void driver::setup_command_line(int argc, char *argv[]) {
  po::positional_options_description pd;
  pd.add("host", 1).add("port", 1);

  // clang-format off
    desc.add_options()
        ("help", "produce help message")
        ("host", po::value<std::string>(&args.listener_host)->default_value(args.listener_host), "listener host address")
        ("port", po::value<std::string>(&args.listener_port)->default_value(args.listener_port), "listener port")
        ("target_host", po::value<std::vector<std::string>>(&args.target_hosts), "target host address")
        ("target_port", po::value<std::vector<std::string>>(&args.target_ports), "target port")
        ("algorithm", po::value<Algorithm::Type>(&args.balancing_algorithm)->default_value(args.balancing_algorithm), 
            "selected balancing algorithm. Available algorithms: constant, round_robin, sticky_round_robin, weighted_round_robin"
            ", ip_hash"
        )
        ("target", po::value<std::size_t>(&args.target_idx), "index of selected target, must be specified if selected algorithm is constant (target >= 0)")
        ("stick_factor", po::value<std::size_t>(&args.stick_factor), "stick factor, must be specified if selected algorithm is sticky_round_robin (stick_factor > 0)")
        ("target_weight", po::value<std::vector<int>>(&args.weights), "weight of the target, must be specified for each target if selected algorithm is weighted_round_robin (weight > 0)");
    po::store(po::command_line_parser(argc, argv).options(desc).positional(pd).run(), vm);
    po::notify(vm);
  // clang-format on
}

bool driver::validate_arguments() const {
  if (!vm.count("host") || !vm.count("port") || !vm.count("target_host") ||
      !vm.count("target_port"))
    return false;
  if (args.target_hosts.size() != args.target_ports.size())
    return false;
  if (args.balancing_algorithm == Algorithm::Type::CONSTANT &&
      (!vm.count("target") ||
       (args.target_idx < 0 || args.target_idx >= args.target_hosts.size())))
    return false;
  return true;
}

driver::driver(int argc, char *argv[]) { setup_command_line(argc, argv); }

int driver::run() {
  if (vm.count("help") || !validate_arguments()) {
    std::cout << "Usage: load_balancer HOST PORT TARGETS" << '\n';
    std::cout << desc;
    return vm.count("help") ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  std::vector<TargetInfo> targets;
  for (std::size_t idx = 0, size = args.target_hosts.size(); idx != size;
       ++idx) {
    targets.push_back(TargetInfo{args.target_hosts[idx], args.target_ports[idx],
                                 args.target_ports[idx] == "443"});
  }
  load_balancer::server{}.run(args.listener_host, args.listener_port,
                              AlgorithmInfo{args.balancing_algorithm,
                                            args.target_idx, args.stick_factor,
                                            args.weights},
                              targets);

  return EXIT_SUCCESS;
}

} // namespace load_balancer

} // namespace eoanermine