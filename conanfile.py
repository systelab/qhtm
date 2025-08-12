from conans import ConanFile, tools

class QhtmLibConan(ConanFile):
    name = "qhtm"
    description = "C++, Win32 HTML control"
    author = "CSW <csw@werfen.com>"
    topics = ("conan", "html")
    license = "MIT"
    generators = "visual_studio"
    settings = "os", "compiler", "build_type", "arch"
    url = "https://albaservidor/svn/extern-libs/trunk/src/qhtm_1.70"
	# uses zlib 1.3.1


    def package(self):
        self.copy("*.h", dst="include/qhtm", src="qhtm")
        self.copy("array.h", dst="include/qhtm")
        self.copy("common.h", dst="include/qhtm")
        self.copy("map.h", dst="include/qhtm")
        self.copy("mapiter.h", dst="include/qhtm")
        self.copy("stack.h", dst="include/qhtm")
        self.copy("windowtext.h", dst="include/qhtm")
        self.copy("winhelper.h", dst="include/qhtm")
        self.copy("*.lib", dst="lib", src=("lib/%s" % self.settings.build_type))
        self.copy("*.pdb", dst="lib", src=("lib/%s" % self.settings.build_type))

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)

