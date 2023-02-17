#pragma once

#include <boost/program_options.hpp>

#include <load_balancer/driver/arguments.hpp>

namespace po = boost::program_options;

namespace eoanermine {

namespace load_balancer {

class driver {
  CLIArguments args;
  po::options_description desc{"Allowed options"};
  po::variables_map vm;

  void setup_command_line(int argc, char *argv[]);
  bool validate_arguments() const;

public:
  driver(int argc, char *argv[]);
  int run();
};

} // namespace load_balancer

} // namespace eoanermine
