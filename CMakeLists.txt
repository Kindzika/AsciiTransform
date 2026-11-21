cmake_minimum_required(VERSION 3.15)
project(unu CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    include(FetchContent)

    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(FT_DISABLE_ZLIB     ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_BZIP2    ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_PNG      ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_BROTLI   ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG        VER-2-13-3
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(freetype)
else()
    find_package(Freetype REQUIRED)
endif()

add_executable(unu main.cpp glyph.cpp)

if(WIN32)
    target_link_libraries(unu PRIVATE freetype)
else()
    target_link_libraries(unu PRIVATE Freetype::Freetype)
endif()

if(MINGW)
    target_link_options(unu PRIVATE -static -static-libgcc -static-libstdc++)
elseif(MSVC)
    set_property(TARGET unu PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
