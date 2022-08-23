project(load_balancer)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.71 COMPONENTS coroutine program_options REQUIRED)

add_executable(load_balancer
	src/main.cpp include/common.hpp include/driver.hpp include/relay.hpp include/server.hpp
	include/algorithms/algorithm.hpp include/algorithms/round_robin.hpp include/algorithms/constant.hpp
)
target_include_directories(load_balancer PRIVATE include/)

target_link_libraries(load_balancer Threads::Threads Boost::coroutine Boost::program_options)
