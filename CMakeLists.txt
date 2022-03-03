cmake_minimum_required(VERSION 3.10)
project(munu_io VERSION 0.1)

find_package(Boost COMPONENTS system thread REQUIRED)
message(STATUS "Boost version  : ${Boost_VERSION}")
message(STATUS "Boost location : ${Boost_INCLUDE_DIRS}")

add_library(munu_io INTERFACE)
target_include_directories(munu_io INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(munu_io INTERFACE
    Boost::system
    Boost::thread
)
target_compile_features(munu_io INTERFACE cxx_std_17)

add_subdirectory(tests)


