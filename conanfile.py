from conan import ConanFile
from conan.tools.files import copy, collect_libs
import os

class QhtmLibConan(ConanFile):
    name = "qhtm"
    description = "C++, Win32 HTML control"
    author = "CSW <csw@werfen.com>"
    topics = ("conan", "html")
    license = "MIT"
    settings = "os", "compiler", "build_type", "arch"
    url = "https://github.com/systelab/qhtm"

    exports_sources = (
        "conanfile.py",
        "*.h",
        "*.inl",
        "qhtm/**",
        "zlib/**",
        "libpng/**",
        "jpeg-6b/**",
        "platform/**",
        "imglib/**",
        "datasource/**",
        "reuse/**",
        "guitools/**",
        "debughlp/**",
        "qhtm.sln",
        "ConanPackage.props",
    )

    def requirements(self):
        self.requires("libjpeg/6b")
        self.requires("libpng/1.2.4")
        self.requires("zlib/1.3.1")

    def package(self):
        copy(self, "*.h", dst=os.path.join(self.package_folder, "include", "qhtm"), src=os.path.join(self.source_folder, "qhtm"))
        copy(self, "array.h", dst=os.path.join(self.package_folder, "include", "qhtm"), src=self.source_folder)
        copy(self, "common.h", dst=os.path.join(self.package_folder, "include", "qhtm"), src=self.source_folder)
        copy(self, "map.h", dst=os.path.join(self.package_folder, "include", "qhtm"), src=self.source_folder)
        copy(self, "mapiter.h", dst=os.path.join(self.package_folder, "include", "qhtm"), src=self.source_folder)
        copy(self, "stack.h", dst=os.path.join(self.package_folder, "include", "qhtm"), src=self.source_folder)
        copy(self, "windowtext.h", dst=os.path.join(self.package_folder, "include", "qhtm"), src=self.source_folder)
        copy(self, "winhelper.h", dst=os.path.join(self.package_folder, "include", "qhtm"), src=self.source_folder)
        copy(self, "*.lib", dst=os.path.join(self.package_folder, "lib"), src=os.path.join(self.source_folder, "lib", str(self.settings.build_type)))
        copy(self, "*.pdb", dst=os.path.join(self.package_folder, "lib"), src=os.path.join(self.source_folder, "lib", str(self.settings.build_type)))

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)

