# How to CMake Good - 2e - Source/Binary Directories and add_subdirectory()
# https://www.youtube.com/watch?v=vyHp2YNUmSQ&t=56s

function(print_variables)
    message(STATUS " ")
    message(STATUS "CMAKE_SOURCE_DIR:           ${CMAKE_SOURCE_DIR}") # CMake SOURCE directory (absolute path to entry point of top level entity)
    message(STATUS "CMAKE_BINARY_DIR:           ${CMAKE_BINARY_DIR}") # CMake BINARY directory (absolute path to build folder (folder where cmake was run from))
    message(STATUS "PROJECT_SOURCE_DIR:         ${PROJECT_SOURCE_DIR}") # SOURCE directory of project '${PROJECT_NAME}', gets updated with call to project()
    message(STATUS "PROJECT_BINARY_DIR:         ${PROJECT_BINARY_DIR}") # BINARY directory of project '${PROJECT_NAME}', gets updated with call to project(). By default (with one argument) the project's relative path is appended to the build directory
    message(STATUS "PROJECT_VERSION:            ${PROJECT_VERSION}") # version of current project
    message(STATUS "CMAKE_CURRENT_SOURCE_DIR:   ${CMAKE_CURRENT_SOURCE_DIR}") # current SOURCE directory, changed by add_subdirectory()
    message(STATUS "CMAKE_CURRENT_BINARY_DIR:   ${CMAKE_CURRENT_BINARY_DIR}") # current BINARY directory, changed by add_subdirectory()
    message(STATUS "CMAKE_CURRENT_LIST_FILE:    ${CMAKE_CURRENT_LIST_FILE}") # current script file (may be the location of an include())
    message(STATUS "CMAKE_CURRENT_LIST_DIR:     ${CMAKE_CURRENT_LIST_DIR}") # current script directory (may be the location of an include())
    message(STATUS "CMAKE_PREFIX_PATH:          ${CMAKE_PREFIX_PATH}")
    message(STATUS " ")
endfunction()
