from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan.tools import files
import pathlib

class ASWPP(ConanFile):
    name = "aswpp"
    description = "C++ wrapper for AngelScript"
    version = "0.1.0"
    author = "Diego Jesus <diego.a.r.jz@gmail.com>"

    settings = "os", "compiler", "arch", "build_type"
    default_settings = (
        "build_type=Release"
    )
    options = {
        "AngelScriptVersion": ["ANY"],
        "with_coverage": [True, False]
    }
    default_options = {
        "AngelScriptVersion": "2.37.0",
        "with_coverage": False
    }

    exports_sources = "CMakeLists.txt", "src/*", "include/*", "third_party/*", "examples/*", "unit_tests/*"

    @property
    def angel_script_version(self):
        return str(self.options.AngelScriptVersion)


    @property
    def angel_script_dest_dir(self):
        root = pathlib.Path(__file__).parent.resolve()
        return f"{root}/third_party/AngelScript"

    def requirements(self):
        self.requires("gtest/1.15.0")
        self.requires("magic_enum/0.9.6")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        self._configure_cmake().build()

    def package(self):
        self._configure_cmake().install()


    def _configure_cmake(self):
        cmake = CMake(self)
        variables = {
            "WITH_COVERAGE": self.options.with_coverage,
            "Angelscript_VERSION": self.angel_script_version
        }
        cmake.configure(variables=variables)
        return cmake
