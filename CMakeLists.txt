# PC port build. The GBA ROM is still built with make; see port/README.md.
cmake_minimum_required(VERSION 3.20)
project(warioland4_pc C ASM)

option(WL4_CAST_WARNINGS "Show the pointer/integer cast warnings (the GbaPtr worklist)" OFF)

# Game code: everything in src/ except the ROM data in src/data/, which the port reads from
# the user's ROM. The function pointer tables there hold native addresses, so they are built.
file(GLOB_RECURSE WL4_GAME_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.c)
list(FILTER WL4_GAME_SOURCES EXCLUDE REGEX "/src/data/")
list(APPEND WL4_GAME_SOURCES
    ${PROJECT_SOURCE_DIR}/src/data/color_fading_pointers.c
    ${PROJECT_SOURCE_DIR}/src/data/stage_end_pointers.c
    ${PROJECT_SOURCE_DIR}/src/data/tile_fn_pointers.c
)

file(GLOB WL4_LAYOUT_ASSERTS CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/port/generated/layout_asserts_*.c)

add_library(wl4_game STATIC ${WL4_GAME_SOURCES} ${WL4_LAYOUT_ASSERTS})
target_include_directories(wl4_game PRIVATE include port/include)
target_compile_definitions(wl4_game PRIVATE PORTABLE VERSION_US)
target_compile_options(wl4_game PRIVATE
    -std=gnu89
    # agbcc's semantics: unsigned char, no type-based aliasing, wrapping signed overflow,
    # and null pointers that may be dereferenced (GBA address 0 is the BIOS).
    -funsigned-char -fno-strict-aliasing -fwrapv -fno-delete-null-pointer-checks
    # GCC bitfield layout, as agbcc uses, instead of the Microsoft layout mingw defaults to.
    -mno-ms-bitfields
    -Werror=return-type -Werror=implicit-function-declaration
    # Pointer type mismatches and int/pointer conversions are fixed by the GbaPtr conversion.
    -Wno-error=incompatible-pointer-types -Wno-error=incompatible-function-pointer-types -Wno-error=int-conversion
)
if(NOT WL4_CAST_WARNINGS)
    target_compile_options(wl4_game PRIVATE
        -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-void-pointer-to-int-cast -Wno-int-to-void-pointer-cast)
endif()

add_library(wl4_platform_stub STATIC port/platform/stub/platform_stub.c)

# The port runtime and the executable. port/generated/gba_*.{c,s} come from
# port/tools/gen_symbols.py: GBA addresses for the game's data, the function dispatch table,
# and stubs for the functions that are still asm.
add_executable(wl4
    port/src/main.c
    port/src/gba_functions.c
    port/src/gba_runtime.c
    port/generated/gba_symbols.s
    port/generated/gba_functions.c
    port/generated/gba_stubs.c
)
target_include_directories(wl4 PRIVATE port/include)
# gba_functions.c declares every game function as void(void) to take its address.
set_source_files_properties(port/generated/gba_functions.c PROPERTIES COMPILE_OPTIONS -Wno-incompatible-library-redeclaration)
target_link_libraries(wl4 PRIVATE wl4_game wl4_platform_stub)
