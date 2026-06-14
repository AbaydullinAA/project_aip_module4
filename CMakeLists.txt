cmake_minimum_required(VERSION 3.16)
project(LinearFit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GSL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(fitting
        main.cpp
        functions.cpp
)

target_link_libraries(fitting PRIVATE GSL::gsl GSL::gslcblas)

enable_testing()
add_executable(test_fitting
        test.cpp
        functions.cpp
)
target_link_libraries(test_fitting PRIVATE GSL::gsl GSL::gslcblas)

add_test(NAME LinearFitTests COMMAND test_fitting)