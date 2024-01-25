@if defined MINILIB_VCPKG_DIR (set "VCPKG_DIR=%MINILIB_VCPKG_DIR%") else (set "VCPKG_DIR=../vcpkg")

cmake.exe -S=./src -B=./build -DVCPKG_TARGET_TRIPLET="x64-windows" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_DIR%/scripts/buildsystems/vcpkg.cmake" -G"Visual Studio 17 2022"