cmake_minimum_required(VERSION 3.20)

add_library(genetic_algorithm
    src/genetic_algorithm.cc
    src/chromosome.cc
    src/individual.cc
    src/selection.cc
    src/mutation.cc
    src/crossover.cc
)
add_library(cshorelark::genetic_algorithm ALIAS genetic_algorithm)

target_include_directories(genetic_algorithm
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(genetic_algorithm PUBLIC cxx_std_17)

target_link_libraries(genetic_algorithm 
    PUBLIC 
        cshorelark::random
        fmt::fmt 
        spdlog::spdlog 
        range-v3::range-v3
        nonstd::span-lite 
        tl::expected
)

# Enable warnings and treat them as errors
if(MSVC)
    target_compile_options(genetic_algorithm PRIVATE /W4 /WX)
else()
    target_compile_options(genetic_algorithm PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Documentation
find_package(Doxygen OPTIONAL_COMPONENTS dot)
if(DOXYGEN_FOUND)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    
    doxygen_add_docs(genetic-algorithm-docs
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        COMMENT "Generating genetic algorithm documentation"
    )
endif()

# Tests
if(BUILD_TESTING)
    find_package(Catch2 REQUIRED)
    
    add_executable(genetic_algorithm_test
        test/genetic_algorithm_test.cc
        test/chromosome_test.cc
        test/individual_test.cc
        test/selection_test.cc
        test/mutation_test.cc
        test/crossover_test.cc
        test/statistics_test.cc
    )
    
    target_include_directories(genetic_algorithm_test
        PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

    target_compile_features(genetic_algorithm_test PUBLIC cxx_std_17)    

    target_link_libraries(genetic_algorithm_test
        PRIVATE
            cshorelark::genetic_algorithm
            nonstd::span-lite 
            tl::expected
            Catch2::Catch2WithMain
    )
    
    # Enable sanitizers in Debug mode
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(genetic_algorithm_test
                PRIVATE
                    -fsanitize=address,undefined
                    -fno-omit-frame-pointer
            )
            target_link_options(genetic_algorithm_test
                PRIVATE
                    -fsanitize=address,undefined
            )
        endif()
    endif()
    
    include(CTest)
    include(Catch)
    catch_discover_tests(genetic_algorithm_test)
endif()

# Install rules
include(GNUInstallDirs)
install(TARGETS genetic_algorithm
    EXPORT genetic_algorithm_targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT genetic_algorithm_targets
    FILE genetic_algorithm_targets.cmake
    NAMESPACE cshorelark::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/genetic_algorithm
) 