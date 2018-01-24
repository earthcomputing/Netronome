#
# Copyright (C) 2018 Earth Computing Inc.  All rights reserved.
#
cmake_minimum_required(VERSION 3.3)
project(entl_sm_test)

set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)

#include(CodeCoverage)

SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} . .. ../alo_unit_test )

add_definitions( -DNETRONOME_HOST )

# Link runTests with what we want to test and the GTest and pthread library
add_executable(runTests entl_sm2_test_main.cpp entl_sm2_tester.c ../alo_unit_test/cyc_random.c ../alo_unit_test/alo_tester.c ../entl_state_machine.c ../atomic_link_op.c)

#set(Coverage_NAME "coverage_target")
#message( "set :"${Coverage_NAME}) 
#setup_target_for_coverage(coverage runTests coverage_report)

target_link_libraries(runTests ${GTEST_LIBRARIES} pthread gcov)
#target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)