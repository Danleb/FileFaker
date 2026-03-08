option(ENABLE_CONAN_INSTALL "" ON)
if(ENABLE_CONAN_INSTALL)
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan_provider.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/develop2/conan_provider.cmake"
                      "${CMAKE_BINARY_DIR}/conan_provider.cmake"
                      TLS_VERIFY ON)
    endif()
			
    set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES "${CMAKE_BINARY_DIR}/conan_provider.cmake")
endif()

cmake_minimum_required(VERSION 3.26)
project(FileFaker VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)
add_compile_options(/W4)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(GTest CONFIG REQUIRED)

add_library(FileFakerStaticLib STATIC)

target_link_libraries(FileFakerStaticLib PUBLIC Pathcch.lib)

add_executable(FileFakerExecutable)
target_link_libraries(FileFakerExecutable FileFakerStaticLib)
set_target_properties(FileFakerExecutable PROPERTIES 
	OUTPUT_NAME "filefaker"
)

add_library(FileFakerClient SHARED)
target_link_libraries(FileFakerClient PUBLIC Pathcch.lib)

enable_testing()

add_subdirectory("src")
add_subdirectory("headers")
add_subdirectory("tests")
