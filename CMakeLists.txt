cmake_minimum_required(VERSION 3.15)
project(C___Test)

set(CMAKE_CXX_STANDARD 14)

add_executable(C___Test main.cpp epollIO.h epollIO.cpp epoll_server.h epoll_server.cpp  threadpool.h threadpool.cpp)

find_package(Threads REQUIRED)
target_link_libraries(C___Test Threads::Threads)