cmake_minimum_required(VERSION 3.20)

add_library(neural_network
    src/network.cc
    src/layer.cc
    src/neuron.cc
)
add_library(cshorelark::neural_network ALIAS neural_network)

target_include_directories(neural_network
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(neural_network PUBLIC cxx_std_17)

target_link_libraries(neural_network
    PUBLIC
        cshorelark::random
        fmt::fmt
        spdlog::spdlog
        date::date
        range-v3::range-v3
        nonstd::span-lite
        tl::expected
)

# Enable warnings and treat them as errors, but allow exceptions
if(MSVC)
    target_compile_options(neural_network PRIVATE /W4 /WX /EHsc)
else()
    target_compile_options(neural_network PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -fexceptions
    )
endif()

# Documentation
find_package(Doxygen OPTIONAL_COMPONENTS dot)
if(DOXYGEN_FOUND)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

    doxygen_add_docs(neural-network-docs
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        COMMENT "Generating neural network documentation"
    )
endif()

# Tests
if(BUILD_TESTING)
    find_package(Catch2 REQUIRED)

    add_executable(neural_network_test
        test/neuron_test.cc
        test/layer_test.cc
        test/network_test.cc
        test/activation_test.cc
    )

    target_link_libraries(neural_network_test
        PRIVATE
            cshorelark::neural_network
            Catch2::Catch2WithMain
    )

    # Enable sanitizers in Debug mode
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(neural_network_test
                PRIVATE
                    -fsanitize=address,undefined
                    -fno-omit-frame-pointer
            )
            target_link_options(neural_network_test
                PRIVATE
                    -fsanitize=address,undefined
            )
        endif()
    endif()

    include(CTest)
    include(Catch)
    catch_discover_tests(neural_network_test)
endif()

# Install rules
include(GNUInstallDirs)
install(TARGETS neural_network
    EXPORT neural_network_targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT neural_network_targets
    FILE neural_network_targets.cmake
    NAMESPACE cshorelark::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/neural_network
)
