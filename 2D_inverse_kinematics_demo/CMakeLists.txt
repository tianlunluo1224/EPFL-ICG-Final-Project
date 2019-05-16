CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(inversekinematics)

FIND_PACKAGE(GLUT)
FIND_PACKAGE(OpenGL)

SET(SRC
src/main.cpp
src/linkedstructure.cpp
)

ADD_EXECUTABLE(inversekinematics ${SRC})
TARGET_LINK_LIBRARIES(inversekinematics ${GLUT_LIBRARY} ${OPENGL_LIBRARY})
