
cmake_minimum_required(VERSION 3.16)
project(main LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

# Add the include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(main src/main.cpp src/Particle.cpp src/ParticleSimulation.cpp src/QuadTree.cpp)

target_link_libraries(main PRIVATE sfml-graphics)
target_compile_features(main PRIVATE cxx_std_17)

# Add optimization and warning flags
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(main PRIVATE -Wall -Wextra -Werror -O3)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(main PRIVATE /W4 /WX /O2)
endif()

if(WIN32)
    add_custom_command(
        TARGET main 
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()

install(TARGETS main)

