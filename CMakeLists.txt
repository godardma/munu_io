cmake_minimum_required(VERSION 3.10)
project(munu_io VERSION 0.1)

find_package(Boost COMPONENTS system thread REQUIRED)
message(STATUS "Boost version  : ${Boost_VERSION}")
message(STATUS "Boost location : ${Boost_INCLUDE_DIRS}")

list(APPEND munu_io_headers
    include/munu_io/AsyncDevice.h
    include/munu_io/AsyncDeviceWritable.h
    include/munu_io/AsyncService.h
    include/munu_io/ClientTCP.h
    include/munu_io/SerialDevice.h
)


add_library(munu_io INTERFACE)
set_target_properties(munu_io PROPERTIES
    INTERFACE_PUBLIC_HEADER "${munu_io_headers}"
)
target_include_directories(munu_io INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(munu_io INTERFACE
    Boost::system
    Boost::thread
)
target_compile_features(munu_io INTERFACE cxx_std_17)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/rtac_installation.cmake)
rtac_install_target(munu_io)

add_subdirectory(tests)


