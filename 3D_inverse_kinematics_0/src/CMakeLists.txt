# OpenGL & GLEW library
find_package(OpenGL)
ADD_DEFINITIONS(-DGLEW_STATIC)

# source files
file(GLOB SOURCES ./*.cpp)
file(GLOB SOURCES_OBJECT ./object/*.cpp)
file(GLOB SOURCES_MESH ./mesh/*.cpp)
file(GLOB HEADERS ./*.h)
file(GLOB HEADERS_OBJECT ./object/*.h)
file(GLOB HEADERS_MESH ./mesh/*.h)
file(GLOB SHADERS ./*.vert ./*.frag)

# Make sure the textures and shaders are available
set(TEXTURE_PATH ${CMAKE_SOURCE_DIR}/textures CACHE PATH "location of texture images")
set(SHADER_PATH  ${CMAKE_SOURCE_DIR}/src      CACHE PATH "location of shader programs")
add_definitions("-DTEXTURE_PATH=\"${TEXTURE_PATH}\"")
add_definitions("-DSHADER_PATH=\"${SHADER_PATH}\"")

# executable
add_executable(InverseKinematics ${HEADERS} ${HEADERS_OBJECT} ${HEADERS_MESH} ${SOURCES} ${SOURCES_OBJECT} ${SOURCES_MESH})
target_include_directories(InverseKinematics SYSTEM PUBLIC ${GLEW_INCLUDE_DIRS})
# Note: target_link_libraries(glfw) should actually bring in the necessary header files.
target_link_libraries(InverseKinematics
    lodePNG
    glfw
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${CMAKE_SOURCE_DIR}/lib/OpenBLAS-0.3.6/release/lib/libopenblas.lib)

add_custom_command(TARGET InverseKinematics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/lib/openBLAS-0.3.6/release/bin/libopenblas.dll $<TARGET_FILE_DIR:InverseKinematics>)
