cmake_minimum_required(VERSION 3.16)
project(ProgramHunter)

set(CMAKE_CXX_STANDARD 20)

add_executable(ProgramHunter main.cpp PlatformDetection.hpp
        Server.cpp
        Server.hpp
        Client.cpp
        Client.hpp
        BaseNetwork.hpp
        ProgramHunter.cpp
        ProgramHunter.hpp)

add_subdirectory(Vendor/enet)

# Dependencies - enet
target_link_libraries(ProgramHunter enet_static)
target_include_directories(ProgramHunter PRIVATE ${VENDOR_DIRECTORY}/enet)