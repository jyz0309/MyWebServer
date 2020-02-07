cmake_minimum_required(VERSION 3.15)
project(C___Test)

set(CMAKE_CXX_STANDARD 14)

add_executable(C___Test main.cpp Socket.h Socket.cpp Epoll.h Epoll.cpp RequestData.h RequestData.cpp UtilFunc.h ThreadPool.h ThreadPool.h ThreadPool.cpp)

find_package(Threads REQUIRED)
target_link_libraries(C___Test Threads::Threads)