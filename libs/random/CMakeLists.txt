cmake_minimum_required(VERSION 3.20)

add_library(random
    src/random.cc
)
add_library(cshorelark::random ALIAS random)

target_include_directories(random
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(random PUBLIC cxx_std_17)

target_link_libraries(random 
    PUBLIC 
        spdlog::spdlog 
        date::date        
        tl::expected
)

# Enable warnings and treat them as errors, but allow exceptions
if(MSVC)
    target_compile_options(random PRIVATE /W4 /WX /EHsc)
else()
    target_compile_options(random PRIVATE 
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
    
    doxygen_add_docs(random-docs
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        COMMENT "Generating random documentation"
    )
endif()

# Tests
if(BUILD_TESTING)
    find_package(Catch2 REQUIRED)
    
    add_executable(random_test
        test/random_test.cc
    )
    
    target_link_libraries(random_test
        PRIVATE
            cshorelark::random
            Catch2::Catch2WithMain
    )
    
    # Enable sanitizers in Debug mode
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(random_test
                PRIVATE
                    -fsanitize=address,undefined
                    -fno-omit-frame-pointer
            )
            target_link_options(random_test
                PRIVATE
                    -fsanitize=address,undefined
            )
        endif()
    endif()
    
    include(CTest)
    include(Catch)
    catch_discover_tests(random_test)
endif()

# Install rules
include(GNUInstallDirs)
install(TARGETS random
    EXPORT random_targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT random_targets
    FILE random_targets.cmake
    NAMESPACE cshorelark::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/random
)