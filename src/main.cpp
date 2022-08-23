#include "driver.hpp"

int main(int argc, char* argv[]) {
    eoanermine::load_balancer::driver driver{argc, argv};
    int result_code = driver.run();
    return result_code;
}