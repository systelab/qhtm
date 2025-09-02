from conan import ConanFile
from conan.tools.files import copy
import os

class LibJPEGConan(ConanFile):
	name = "libjpeg"
	version = "6b"
	homepage = "https://repository.timesys.com/buildsources/l/libjpeg/libjpeg-6b/"
	
	def export_sources(self):
		copy(self, pattern="*", dst=self.export_sources_folder, src=self.recipe_folder, excludes="*.py")

	def package(self):
		copy(self, "*.h", dst=os.path.join(self.package_folder, "include"), src=self.source_folder, excludes="*.py")