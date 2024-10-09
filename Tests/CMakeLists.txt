cmake_minimum_required(VERSION 3.15.3)
project(tests CXX C)

set(TESTS_DIR ${CMAKE_CURRENT_LIST_DIR})
cmake_path(GET TESTS_DIR PARENT_PATH ROOT_DIR)
set(UNIT_TESTS_DIR ${ROOT_DIR}/Tests)

# Check if the COVERAGE flag is set
option(COVERAGE "Enable coverage flags" OFF)
option(TEST_NUM "Enable test num" OFF)
if(COVERAGE EQUAL 1)
    message(STATUS "Code coverage enabled!")
    add_compile_options(-g -O0 --coverage)
    add_link_options(--coverage)
endif()

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3)
include_directories(${FFTW_INCLUDE_DIRS})
FILE(GLOB commonSources       ${ROOT_DIR}/Src/common/FFT.cpp
                              ${ROOT_DIR}/Src/common/algorithms.cpp)

function(gen_test app_name test_file)
    add_executable(${app_name}
                   ${commonSources}
                   ${test_file})

    target_include_directories(${app_name} PUBLIC ${ROOT_DIR}/Src/common ${ROOT_DIR}/Src/platform/ubuntu)
    
    target_link_libraries(${app_name} GTest::gtest GTest::gtest_main ${FFTW_LIBRARIES} m stdc++)
    
endfunction()

gen_test(fft     ${UNIT_TESTS_DIR}/common/test_fft.cpp)
gen_test(algorithms ${UNIT_TESTS_DIR}/common/test_algorithms.cpp)
