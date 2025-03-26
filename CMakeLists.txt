cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

if(MSVC)
    add_compile_options(/EHsc)
endif()

project(vulkan)

# Find source files and headers
file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)
file(GLOB_RECURSE SHADERS src/Shaders/*.spv)

# Create the executable
add_executable(vulkan ${SRC_FILES} ${HEADERS})
file(COPY ${SHADERS} DESTINATION ${CMAKE_BINARY_DIR}/Shaders)

# IMPORT GLFW
if(DEFINED ENV{GLFW})
    target_include_directories(vulkan PRIVATE $ENV{GLFW}/include)
    target_link_directories(vulkan PRIVATE $ENV{GLFW}/lib-vc2022)
    target_link_libraries(vulkan PRIVATE glfw3.lib)
    message(STATUS "Using GLFW include directory: $ENV{GLFW}/include")
else()
    message(WARNING "GLFW environment variable not set. Include directories might be missing.")
endif()

# IMPORT GLM
if(DEFINED ENV{GLM})
    target_include_directories(vulkan PRIVATE $ENV{GLM})
    message(STATUS "Using GLM include directory: $ENV{GLM}")
else()
    message(WARNING "GLM environment variable not set. Include directories might be missing.")
endif()

# IMPORT VULKAN
if(DEFINED ENV{VULKAN})
    target_include_directories(vulkan PRIVATE $ENV{VULKAN}/include)
    target_link_directories(vulkan PRIVATE $ENV{VULKAN}/Lib)
    target_link_libraries(vulkan PRIVATE vulkan-1.lib)
    message(STATUS "Using VULKAN include directory: $ENV{VULKAN}/include")
else()
    message(WARNING "VULKAN environment variable not set. Include directories might be missing.")
endif()
