project(load_balancer)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.71 COMPONENTS coroutine program_options REQUIRED)

find_package(OpenSSL 3.0)

add_executable(load_balancer
	load_balancer/main.cpp load_balancer/driver/driver.cpp load_balancer/server/server.cpp
	load_balancer/server/algorithms/algorithm.cpp load_balancer/server/algorithms/constant.cpp load_balancer/server/algorithms/round_robbin.cpp
	load_balancer/common.hpp load_balancer/driver/driver.hpp load_balancer/driver/arguments.hpp
	load_balancer/server/relay.hpp load_balancer/server/server.hpp load_balancer/server/algorithms/algorithm.hpp
	load_balancer/server/algorithms/round_robin.hpp load_balancer/server/algorithms/constant.hpp
)
target_include_directories(load_balancer PRIVATE .)

target_link_libraries(load_balancer OpenSSL::SSL Threads::Threads Boost::coroutine Boost::program_options)
