from conan import ConanFile
from conan.tools.cmake import CMake
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
        "AngelScriptVersion": ["ANY"]
    }
    default_options = {
        "AngelScriptVersion": "2.37.0"
    }
    generators = (
        "CMakeDeps",
        "CMakeToolchain"
    )

    @property
    def angel_script_version(self):
        return str(self.options.AngelScriptVersion)


    @property
    def angel_script_url(self):
        version = self.angel_script_version
        return f"https://www.angelcode.com/angelscript/sdk/files/angelscript_{version}.zip"


    @property
    def angel_script_dest_dir(self):
        root = pathlib.Path(__file__).parent.resolve()
        return f"{root}/third_party/AngelScript"

    def requirements(self):
        self.requires("gtest/1.15.0")

    def build(self):
        files.get(self,
                  url=self.angel_script_url,
                  destination=self.angel_script_dest_dir)
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
