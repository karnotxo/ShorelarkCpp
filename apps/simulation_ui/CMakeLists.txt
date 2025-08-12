cmake_minimum_required(VERSION 3.20)

add_executable(simulation_ui
    src/app.cc
    src/app.h
    src/imgui_context.cc
    src/imgui_context.h
    src/main.cc
    src/simulation_config.cc
    src/simulation_config.h
    src/simulation_window.cc
    src/simulation_window.h
    src/imgui_stdlib.h
    src/imgui_stdlib.cpp
    src/ImFileDialog.h
    src/ImFileDialog.cpp
    ${PROJECT_SOURCE_DIR}/bindings/imgui_impl_glfw.cpp
    ${PROJECT_SOURCE_DIR}/bindings/imgui_impl_opengl3.cpp
)

add_executable(cshorelark::simulation_ui ALIAS simulation_ui)

target_include_directories(simulation_ui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/bindings
)

target_link_libraries(simulation_ui
    PRIVATE
        cshorelark::random
        cshorelark::simulation
        cshorelark::neural_network
        cshorelark::genetic_algorithm
        stb::stb
        imgui::imgui
        glfw
        GLEW::GLEW
        transwarp::transwarp
        taywee::args
        fmt::fmt
        spdlog::spdlog
        date::date
        range-v3::range-v3
        nonstd::span-lite
        tl::expected
        tomlplusplus::tomlplusplus
)

target_compile_features(simulation_ui PRIVATE cxx_std_17)

# Enable warnings
if(MSVC)
    target_compile_options(simulation_ui PRIVATE /W4)
else()
    target_compile_options(simulation_ui PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Enable sanitizers in Debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(simulation_ui
            PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
        )
        target_link_options(simulation_ui
            PRIVATE
                -fsanitize=address,undefined
        )
    endif()
endif()

# Copy icon assets to build directory
file(GLOB ICON_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/*.png")
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assets/icons)
foreach(ICON_FILE ${ICON_FILES})
    get_filename_component(ICON_NAME ${ICON_FILE} NAME)
    configure_file(${ICON_FILE} ${CMAKE_CURRENT_BINARY_DIR}/assets/icons/${ICON_NAME} COPYONLY)
endforeach()

# Make sure assets directory is included in the installer
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets
        DESTINATION bin
        COMPONENT applications)

# Install the executable
install(TARGETS simulation_ui
    RUNTIME DESTINATION bin
    COMPONENT applications)

# Add tests if enabled
if(BUILD_TESTING)
    find_package(Catch2 REQUIRED)
    
    add_executable(simulation_ui_test
        test/simulation_config_test.cc
        src/simulation_config.cc
        src/simulation_config.h
    )
    
    target_link_libraries(simulation_ui_test
        PRIVATE
            cshorelark::random
            cshorelark::simulation
            cshorelark::neural_network
            cshorelark::genetic_algorithm
            Catch2::Catch2WithMain
            fmt::fmt
            spdlog::spdlog
            date::date
            range-v3::range-v3
            nonstd::span-lite
            tl::expected
            tomlplusplus::tomlplusplus
    )
    
    target_compile_features(simulation_ui_test PRIVATE cxx_std_17)
    
    # Enable sanitizers in Debug mode for tests
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(simulation_ui_test
                PRIVATE
                    -fsanitize=address,undefined
                    -fno-omit-frame-pointer
            )
            target_link_options(simulation_ui_test
                PRIVATE
                    -fsanitize=address,undefined
            )
        endif()
    endif()
    
    include(CTest)
    include(Catch)
    catch_discover_tests(simulation_ui_test)
endif()