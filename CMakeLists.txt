cmake_minimum_required(VERSION 3.16)
project(ProgramHunter)

set(CMAKE_CXX_STANDARD 20)

set(NETWORK_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Network)
set(VENDOR_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Vendor)

add_executable(ProgramHunter main.cpp)

add_subdirectory(Vendor/enet)
add_subdirectory(Foundation)
add_subdirectory(Network)

target_link_libraries(ProgramHunter Network)