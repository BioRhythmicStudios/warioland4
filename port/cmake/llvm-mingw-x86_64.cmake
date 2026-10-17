# Toolchain file for 64-bit Windows builds with llvm-mingw (https://github.com/mstorsjo/llvm-mingw).
# Point LLVM_MINGW at the unpacked release, either as a cache variable or an environment variable:
#   cmake -S . -B build_pc -G Ninja --toolchain port/cmake/llvm-mingw-x86_64.cmake -DLLVM_MINGW=C:/path/to/llvm-mingw

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

if(NOT LLVM_MINGW)
    set(LLVM_MINGW "$ENV{LLVM_MINGW}")
endif()
if(NOT LLVM_MINGW)
    message(FATAL_ERROR "Set LLVM_MINGW to the llvm-mingw directory")
endif()
set(LLVM_MINGW "${LLVM_MINGW}" CACHE PATH "llvm-mingw directory")
# try_compile projects read this file again; pass the setting on to them.
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES LLVM_MINGW)

set(_triple x86_64-w64-mingw32)
if(CMAKE_HOST_WIN32)
    set(_exe .exe)
endif()
set(CMAKE_C_COMPILER "${LLVM_MINGW}/bin/${_triple}-clang${_exe}")
set(CMAKE_CXX_COMPILER "${LLVM_MINGW}/bin/${_triple}-clang++${_exe}")
set(CMAKE_RC_COMPILER "${LLVM_MINGW}/bin/${_triple}-windres${_exe}")
set(CMAKE_AR "${LLVM_MINGW}/bin/llvm-ar${_exe}")
set(CMAKE_RANLIB "${LLVM_MINGW}/bin/llvm-ranlib${_exe}")

set(CMAKE_FIND_ROOT_PATH "${LLVM_MINGW}/${_triple}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
