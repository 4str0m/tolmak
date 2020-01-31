cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(tolmak)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

# Enable C++14 mode on GCC / Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

# include(CheckCXXCompilerFlag)
# include(CheckCXXSourceRuns)

macro(CHECK_CXX_COMPILER_AND_LINKER_FLAGS _RESULT _CXX_FLAGS _LINKER_FLAGS)
  set(CMAKE_REQUIRED_FLAGS ${_CXX_FLAGS})
  set(CMAKE_REQUIRED_LIBRARIES ${_LINKER_FLAGS})
  set(CMAKE_REQUIRED_QUIET TRUE)
  check_cxx_source_runs("#include <iostream>\nint main(int argc, char **argv) { std::cout << \"test\"; return 0; }" ${_RESULT})
  set(CMAKE_REQUIRED_FLAGS "")
  set(CMAKE_REQUIRED_LIBRARIES "")
endmacro()

# Compile GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_INSTALL OFF CACHE BOOL " " FORCE)
set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)
set(GLFW_USE_CHDIR OFF CACHE BOOL " " FORCE)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Quench annoying deprecation warnings when compiling GLFW on OSX
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw"  "ext_build/glfw")

# Two targets have now been defined: `glfw_objects`, which will be merged into
# NanoGUI at the end, and `glfw`.  The `glfw` target is the library itself
# (e.g., libglfw.so), but can be skipped as we do not need to link against it
# (because we merge `glfw_objects` into NanoGUI).  Skipping is required for
# XCode, but preferable for all build systems (reduces build artifacts).
# set_target_properties(glfw PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)

include_directories(ext/glfw/include)
include_directories(ext/glad/include)
include_directories(ext/imgui)
include_directories(ext/stb)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/glm"  "ext_build/glm")
include_directories(ext/glm)

# Prefer libc++ in conjunction with Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  if (CMAKE_CXX_FLAGS MATCHES "-stdlib=libc\\+\\+")
    message(STATUS "NanoGUI: using libc++.")
  else()
  CHECK_CXX_COMPILER_AND_LINKER_FLAGS(HAS_LIBCPP "-stdlib=libc++" "-stdlib=libc++")
  if (HAS_LIBCPP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++")
      message(STATUS "NanoGUI: using libc++.")
  else()
    message(STATUS "NanoGUI: NOT using libc++.")
  endif()
  endif()
endif()

# Compile tolmak with various compiler warnings turned on
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-anonymous-struct -Wno-c99-extensions -Wno-nested-anon-types")
  endif()
endif()

include_directories(include)
file(GLOB imgui_SRC
    ext/imgui/*.h
    ext/imgui/*.cpp
    ext/imgui/examples/imgui_impl_glfw.h
    ext/imgui/examples/imgui_impl_glfw.cpp
    ext/imgui/examples/imgui_impl_opengl3.h
    ext/imgui/examples/imgui_impl_opengl3.cpp
)

file(GLOB shader_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/*.glsl
)

execute_process(
  COMMAND clang++-6.0 ../src/shader_parser.cpp -o shader_parser
)

execute_process(
  COMMAND ./shader_parser ${shader_SRC}
  OUTPUT_FILE ../include/materials.h
)

add_executable(tolmak
  src/main.cpp

  src/mesh_io.cpp include/mesh_io.h
  src/shader.cpp include/shader.h
  src/texture.cpp include/texture.h
  src/light.cpp include/light.h
  src/mesh.cpp include/mesh.h
  include/orbit_camera.h
  # include/material.h
  include/materials.h
  include/common.h

  ext/glad/src/glad.c
  ${imgui_SRC}

  ${shader_SRC}
)

target_link_libraries(tolmak glfw)

set_target_properties(tolmak PROPERTIES OUTPUT_NAME "tolmak")
