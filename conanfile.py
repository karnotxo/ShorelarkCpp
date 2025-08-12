#!/user/bin/env python3
import re
import os
from os.path import join as join_paths
import sys

from conan import ConanFile
from conan.tools.meson import Meson
from conan.tools.cmake import CMakeToolchain, CMakeDeps, CMake, cmake_layout
from conan.tools.gnu import PkgConfigDeps
from conan.errors import ConanInvalidConfiguration
from conan.tools.files import load, copy

if sys.version_info[0] < 3:
    raise Exception("The version of Python must be 3 or greater.")

def get_version():
    try:
        content = load("CMakeLists.txt")
        version = re.search(b"set\(SERVER_CPP_VERSION (.*)\)", content).group(1)
        return version.strip()
    except Exception as e:
        return None
    
class ConanFileServerCpp(ConanFile):
    name = 'server_cpp'
    version = get_version()
    description = "Tool used to replay radar data recording files"
    generators = "CMakeToolchain", "CMakeDeps", "MesonToolchain", "PkgConfigDeps"
    exports_sources = '*', '!bin/*', '!build/*', '!cmake-build-*', '!.idea/*', '!.vscode/*', '!.ci/*', '!.devcontainer/*'
    settings = "os", "compiler", "build_type", "arch"
    options = { 
        "builder": ["meson", "cmake", "qmake"],
        "build_docs": [True, False]
    }
    default_options = {
        "builder": "cmake",
        "build_docs": False
    }
    topics = ('conan', 'meson', 'mesonbuild', 'cmake', 'cmakebuild', 'build-system', 'c11')
    license = "MIT"

    # Función necesaria para distinguir entre requisitos de la versión con c++11 (solo terminal) o 17 o mayor, con fxTUI
    def requirements(self):
        cppstd = self.settings.compiler.get_safe("cppstd")
        # Dependencias comunes
        # Must be greater or equal to standard c++ 17
        self.requires("imgui/1.92.0-docking")
        self.requires("glfw/3.4")
        self.requires("glew/2.2.0")
        self.requires("asio/1.34.2")
        #self.requires("poco/1.12.5p2")
        self.requires("units/2.3.3")
        self.requires("nlohmann_json/3.12.0")
        self.requires("span-lite/0.11.0")
        self.requires("tl-expected/1.1.0")
        self.requires("range-v3/0.12.0") #c++17 compatible
        self.requires("transwarp/2.2.3")
        self.requires("freetype/2.13.2")
        self.requires("tinyxml2/11.0.0")
        self.requires("date/3.0.4")
        #self.requires("string-view-lite/1.8.0") #c++17 compatible
        #self.requires("tl-optional/1.1.0") #c++17 compatible
        #self.requires("ghc-filesystem/1.5.14")
        self.requires("taywee-args/6.4.6")
        self.requires("concurrentqueue/1.0.4")
        self.requires("lunasvg/3.0.1")
        self.requires("tomlplusplus/3.4.0")
        self.requires("stb/cci.20240531")
        self.requires("spdlog/1.15.3")
        self.requires("libenvpp/1.5.0")
		#self.requires("magic_enum/0.9.5")
        self.requires("fmt/11.2.0", override=True)
		#for testing
        self.requires("catch2/3.8.1")
        self.requires("fakeit/2.4.1")

        # Documentation dependencies (only when building docs)
        if self.options.build_docs:
            self.requires("doxygen/1.14.0")
            self.tool_requires("sphinx/7.1.1")
            self.tool_requires("breathe/4.35.0")
            self.tool_requires("sphinx-rtd-theme/1.3.0")
            self.tool_requires("sphinxcontrib-mermaid/0.9.2")
            self.tool_requires("sphinx-multiversion/0.2.4")

    def configure_cmake(self):
        cmake = CMake(self)
        # Configure CMake with modern Conan 2.x style
        cmake.configure(variables={
            'CMAKE_EXPORT_COMPILE_COMMANDS': 'ON'
        })
        return cmake
    # end of method configure_cmake

    def build(self):
        if self.options.get_safe("builder") == "meson":
            defs = dict()
            #defs["-Dconf_target_opt"] = self.options.confTarget
            #defs["-DTRANSWARP_CPP11"] = "TRANSWARP_CPP11"
            meson = Meson(self)
            #meson.configure(build_folder="build")
            meson.configure(cache_build_folder="build", defs=defs)
            #meson.configure()
            meson.build()
            #meson.install()
        elif self.options.builder == "cmake":
            cmake = self.configure_cmake()
            cmake.build()
            #cmake.install()
        elif self.options.builder == "qmake":
            print("Not implemented yet")
    # end of method build

    def generate(self):
        # Copy imgui bindings
        copy(self, "*glfw*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "bindings"))
        copy(self, "*opengl3*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "bindings"))
    # end of method generate
    
    def layout(self):
        if self.options.builder == "cmake":   
            cmake_layout(self)
    # end of method layout
			
    def package(self):	
        if self.options.get_safe("builder") == "meson":
            meson = Meson(self)
            meson.install()
        elif self.options.builder == "cmake":
            cmake = CMake(self)
            cmake.install()
    # end of method package

    def config_options(self):
        if self.settings.get_safe("os") == "Windows":
            self.options.rm_safe("fPIC")
    # end of method config_options

    def configure(self):	        
        self.options["fmt"].header_only = False
        self.options["spdlog"].header_only = False
        self.options["spdlog"].wchar_support = False
        self.options["spdlog"].wchar_filenames = False
        self.options["Poco"].shared = False
        self.options["dear-imgui"].shared = False
    # end of method configure

# end of class ConanFileServerCpp