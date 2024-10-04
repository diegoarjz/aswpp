from conan import ConanFile
from conan.tools.files import save, load
from conan.tools.gnu import AutotoolsToolchain, AutotoolsDeps
from conan.tools.microsoft import unix_path, VCVars, is_msvc
from conan.errors import ConanInvalidConfiguration
from conan.errors import ConanException

class ASWPP(ConanFile):
    name = "aswpp"
    description = "C++ wrapper for AngelScript"
    version = "0.1.0"
    author = "Diego Jesus <diego.a.r.jz@gmail.com>"

    settings = "os", "compiler", "arch", "build_type"
    default_settings = (
        "build_type=Release"
    )
    generators = (
        "CMakeDeps",
        "CMakeToolchain"
    )

    def requirements(self):
        self.requires("gtest/1.15.0")
