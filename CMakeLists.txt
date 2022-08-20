project(load_balancer)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Boost 1.80 COMPONENTS coroutine program_options REQUIRED)

add_executable(load_balancer src/main.cpp)

target_link_libraries(load_balancer Threads::Threads Boost::coroutine Boost::program_options)
