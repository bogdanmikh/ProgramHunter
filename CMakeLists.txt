cmake_minimum_required(VERSION 3.16)
project(ProgramHunter)

set(CMAKE_CXX_STANDARD 20)

set(NETWORK_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Network)
set(VENDOR_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Vendor)
if (WIN32)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    elseif(MINGW OR CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
    endif ()
endif ()

add_executable(ProgramHunter main.cpp)

add_subdirectory(Vendor/enet)
add_subdirectory(Foundation)
add_subdirectory(Network)

target_link_libraries(ProgramHunter Network)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.txt DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.txt DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build/)