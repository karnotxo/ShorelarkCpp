cmake_minimum_required(VERSION 3.20)

add_library(simulation
    src/statistics.cc
    src/animal_individual.cc
    src/animal.cc
    src/brain.cc
    src/eye.cc
    src/food.cc
    src/world.cc
    src/simulation.cc
    src/simulation_error.cc
)

add_library(cshorelark::simulation ALIAS simulation)

target_include_directories(simulation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(simulation
    PUBLIC
        cshorelark::random
        cshorelark::neural_network
        cshorelark::genetic_algorithm
        fmt::fmt spdlog::spdlog date::date range-v3::range-v3 nonstd::span-lite tl::expected
)


target_compile_features(simulation PUBLIC cxx_std_17)

# Tests
if(BUILD_TESTING)
    find_package(Catch2 REQUIRED)
    
    add_executable(simulation-test
        test/vector2d_test.cc
        test/config_test.cc
        test/statistics_test.cc
        test/animal_individual_test.cc
        test/brain_test.cc
        test/animal_test.cc
        test/eye_test.cc
        test/food_test.cc
        test/world_test.cc
    )
    
    target_link_libraries(simulation-test
        PRIVATE
            cshorelark::simulation
            Catch2::Catch2WithMain
    )
    
    # Enable sanitizers in Debug mode
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(simulation-test
                PRIVATE
                    -fsanitize=address,undefined
                    -fno-omit-frame-pointer
            )
            target_link_options(simulation-test
                PRIVATE
                    -fsanitize=address,undefined
            )
        endif()
    endif()
    
    include(CTest)
    include(Catch)
    catch_discover_tests(simulation-test)
endif()