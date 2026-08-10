cmake_minimum_required(VERSION 3.20)

project(MiniCuTe
    VERSION 0.1.0
    DESCRIPTION "A small C++20 CuTe-like layout library"
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(minicute INTERFACE)
add_library(MiniCuTe::minicute ALIAS minicute)

target_compile_features(minicute INTERFACE cxx_std_20)
target_include_directories(minicute INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

# The public file layout follows CuTe's broad split: containers, algorithms,
# integer vocabulary, and IntTuple operations.
set(MINICUTE_HEADERS
    include/minicute.hpp
    include/minicute/algorithm/tuple_algorithms.hpp
    include/minicute/layout_composed.hpp
    include/minicute/container/tuple.hpp
    include/minicute/container/detail/tuple_access.hpp
    include/minicute/container/detail/tuple_algorithms.hpp
    include/minicute/container/detail/tuple_compare.hpp
    include/minicute/container/detail/tuple_fwd.hpp
    include/minicute/container/detail/tuple_storage.hpp
    include/minicute/container/detail/tuple_traits.hpp
    include/minicute/util/type_traits.hpp
    include/minicute/int_tuple.hpp
    include/minicute/stride.hpp
    include/minicute/numeric/integral_constant.hpp
    include/minicute/numeric/integer_sequence.hpp
    include/minicute/layout.hpp
    include/minicute/tensor.hpp
    include/minicute/tiling.hpp)
target_sources(minicute INTERFACE ${MINICUTE_HEADERS})

function(minicute_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

function(minicute_add_test target source)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE MiniCuTe::minicute)
    minicute_enable_warnings(${target})
    add_test(NAME ${target} COMMAND ${target})
endfunction()

include(CTest)
option(MINICUTE_BUILD_TESTS "Build the implemented stage tests" ON)
option(MINICUTE_BUILD_EXAMPLES "Build the final demo" OFF)
option(MINICUTE_BUILD_TODO_STAGES
    "Build stage4/stage5 tests, which still contain assignment TODOs" OFF)

if(BUILD_TESTING AND MINICUTE_BUILD_TESTS)
    minicute_add_test(minicute_stage0 tests/stage0_compile.cpp)
    minicute_add_test(minicute_stage1 tests/stage1_core.cpp)
    minicute_add_test(minicute_stage2 tests/stage2_layout.cpp)
    minicute_add_test(minicute_stage3 tests/stage3_tensor.cpp)
    minicute_add_test(minicute_tiling_tensor_wrappers
                      tests/tiling_tensor_wrappers.cpp)

    if(MINICUTE_BUILD_TODO_STAGES)
        minicute_add_test(minicute_stage4 tests/stage4_algebra.cpp)
        minicute_add_test(minicute_stage5 tests/stage5_tiling.cpp)
    endif()
endif()

if(MINICUTE_BUILD_EXAMPLES)
    add_executable(minicute_final_demo examples/final_demo.cpp)
    target_link_libraries(minicute_final_demo PRIVATE MiniCuTe::minicute)
    minicute_enable_warnings(minicute_final_demo)
endif()
