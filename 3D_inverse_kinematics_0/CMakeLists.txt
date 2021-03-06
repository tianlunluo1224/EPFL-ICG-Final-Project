cmake_minimum_required(VERSION 3.12)

project(InverseKinematics)

include(PrintVariables.cmake)
print_variables()

# set output paths
set(PROJECT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

if(WIN32)
  MESSAGE("CMAKE_PREFIX_PATH: " $CMAKE_PREFIX_PATH)
  set(CMAKE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/src/ ${CMAKE_SOURCE_DIR}/lib/win7)
endif()

# default to Release builds
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# compiler flags
if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

# attempt to find the system's GLEW; build the included one if unsuccessful
find_package(GLEW QUIET)
if (GLEW_FOUND)
  MESSAGE("Using system GLEW")
  set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS}/GL) # At least on Ubuntu, system glew.h is actually in GL/
  link_libraries(${GLEW_LIBRARIES})
else()
  MESSAGE("System GLEW not found... falling back to local GLEW")
  add_subdirectory(lib/glew) # Will look for CMakeLists.txt of GLEW there
  set(GLEW_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/lib/glew)
  set(GLEW_LIBRARIES glew)
endif()

# Attempt to find the system's GLFW; build the included one if unsuccessful
find_package(glfw3 QUIET)
if (glfw3_FOUND)
  MESSAGE("Using system GLFW")
  # Note: target_link_libraries(glfw) performed in src/CMakeLists.txt should
  # actually bring in the necessary header files.
else()
  MESSAGE("System GLFW not found... falling back to local GLFW")
  include_directories(${CMAKE_SOURCE_DIR}/lib/glfw/include/)
  add_subdirectory(lib/glfw)
endif()

# add source directory to include path
include_directories(${CMAKE_SOURCE_DIR}/src/)
include_directories(${CMAKE_SOURCE_DIR}/lib/lodePNG/)
include_directories(${CMAKE_SOURCE_DIR}/lib/OpenBLAS-0.3.6/release/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/Armadillo-9.400.3)


# build source directory
add_subdirectory(lib/lodePNG)
add_subdirectory(src)

# documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
	# prepare doxygen configuration file
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	# add doc as target
	add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
endif(DOXYGEN_FOUND)
