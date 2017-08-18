cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(BISON)
find_package(FLEX)
find_package(GLPK)

BISON_TARGET (citip_parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cxx
              COMPILE_FLAGS --defines=${CMAKE_CURRENT_BINARY_DIR}/parser.hxx)
FLEX_TARGET  (citip_scanner scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.cxx
              COMPILE_FLAGS --header-file=${CMAKE_CURRENT_BINARY_DIR}/scanner.hxx)
ADD_FLEX_BISON_DEPENDENCY(citip_scanner citip_parser)


include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(Citip
    main.cpp
    citip.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.cxx
)
target_compile_options(Citip PUBLIC -std=c++11)
target_link_libraries(Citip glpk)
