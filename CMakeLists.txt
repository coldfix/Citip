cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(BISON)
find_package(FLEX)
find_package(GLPK)

BISON_TARGET (citip_parser parser.y parser.cxx)
FLEX_TARGET  (citip_scanner scanner.l scanner.cxx)
ADD_FLEX_BISON_DEPENDENCY(citip_scanner citip_parser)


include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(Citip
    main.cpp
    citip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parser.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cxx
)
target_link_libraries(Citip glpk)
