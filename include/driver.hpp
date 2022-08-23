#pragma once

#include <boost/program_options.hpp>
#include <string_view>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "server.hpp"

namespace eoanermine {

namespace load_balancer {

class driver {
    std::string listener_host, listener_port;
    std::vector<std::string> target_host, target_port;

    Algorithm::Type balancing_algorithm;
    std::size_t targetIdx;

    po::options_description desc{"Allowed options"};
    po::variables_map vm;

    void setup_command_line(int argc, char* argv[]) {
        po::positional_options_description pd;
        pd.add("host", 1).add("port", 1);

        desc.add_options()
            ("help", "produce help message")
            ("host", po::value<std::string>(&listener_host)->default_value("127.0.0.1"), "listener host address")
            ("port", po::value<std::string>(&listener_port)->default_value("80"), "listener port")
            ("target_host", po::value<std::vector<std::string>>(&target_host), "target host address")
            ("target_port", po::value<std::vector<std::string>>(&target_port), "target port")
            ("algorithm", po::value<Algorithm::Type>(&balancing_algorithm)->default_value(Algorithm::Type::ROUND_ROBIN, "round_robin"), "selected balancing algorithm. Available algorithms: round_robin, constant")
            ("target", po::value<std::size_t>(&targetIdx), "selected target, must be specified if selected algorithm is constant");
        po::store(po::command_line_parser(argc, argv).options(desc).positional(pd).run(), vm);
        po::notify(vm);
    }

    bool validate_arguments() const {
        if (!vm.count("host") || !vm.count("port") || !vm.count("target_host") || !vm.count("target_port")) return false;
        if (target_host.size() != target_port.size()) return false;
        if (balancing_algorithm == Algorithm::Type::CONSTANT && (!vm.count("target") || (targetIdx < 0 || targetIdx >= target_host.size())))
            return false;
        return true;
    }
public:
    driver(int argc, char* argv[]) {
        setup_command_line(argc, argv);
    }

    int run() {
        if (vm.count("help") || !validate_arguments()) {
            std::cout << "Usage: load_balancer HOST PORT TARGETS" << '\n';
            std::cout << desc;
            return vm.count("help") ? EXIT_SUCCESS : EXIT_FAILURE;
        }

        std::vector<std::pair<std::string_view, std::string_view>> targets;
        for (std::size_t idx = 0, size = target_host.size(); idx != size; ++idx) {
            targets.push_back(std::make_pair(std::string_view(target_host[idx]), std::string_view(target_port[idx])));
        }
        load_balancer::server{}.run(listener_host, listener_port, AlgorithmInfo { balancing_algorithm, targetIdx }, targets);

        return EXIT_SUCCESS;
    }
};

} // namespace load_balancer

} // namespace eoanermine
