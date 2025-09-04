conan export-pkg . --name=qhtm --version=1.8.0 --profile:host=.conanprofile -s build_type=Debug -of int
conan export-pkg . --name=qhtm --version=1.8.0 --profile:host=.conanprofile -s build_type=Release -of int

conan export-pkg libpng/ --name=libpng --version=1.2.4 --profile:host=.conanprofile -s build_type=Debug
conan export-pkg libpng/ --name=libpng --version=1.2.4 --profile:host=.conanprofile -s build_type=Release
conan export-pkg jpeg-6b/ --name=libjpeg --version=6b --profile:host=.conanprofile -s build_type=Debug
conan export-pkg jpeg-6b/ --name=libjpeg --version=6b --profile:host=.conanprofile -s build_type=Release