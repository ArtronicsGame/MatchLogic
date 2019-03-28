cmake_minimum_required (VERSION 3.1)
project (Match)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)
find_package(nlohmann_json 3.6.1 REQUIRED)

set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set (Match_VERSION_MAJOR 0)
set (Match_VERSION_MINOR 1)
  
add_executable(Match main.cpp Utils/AtomicQueue.h)

target_link_libraries (Match Box2D Threads::Threads nlohmann_json::nlohmann_json)   