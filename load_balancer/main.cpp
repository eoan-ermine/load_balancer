#include <load_balancer/driver/driver.hpp>

int main(int argc, char *argv[]) {
  eoanermine::load_balancer::driver driver{argc, argv};
  const int result_code = driver.run();
  return result_code;
}