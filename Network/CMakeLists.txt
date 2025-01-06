file(GLOB_RECURSE INCLUDE *.hpp)
file(GLOB_RECURSE SOURCES *.cpp)

add_library(Network ${INCLUDE} ${SOURCES})

target_include_directories(Network PRIVATE ${NETWORK_DIRECTORY})

# Dependencies - enet
target_link_libraries(Network enet_static)
target_include_directories(Network PRIVATE ${VENDOR_DIRECTORY}/enet)

# Dependencies - Foundation
target_link_libraries(Network Foundation)