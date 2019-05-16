// Inverse Kinematics Application by Alexandros Dermenakis

// Link with: opengl32.lib, glu32.lib, glut32.lib.

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#endif

#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <strstream>
#include <iomanip>
#include "linkedstructure.h"
#include "link.h"
#include "Eigen/Dense"

using namespace Eigen;
using namespace std;

// Initial size of graphics window.
const int WIDTH  = 600;
const int HEIGHT = 400;

// Current size of window.
int width  = WIDTH;
int height = HEIGHT;

// Mouse positions, normalized to [0,1].
double xMouse = 0.5;
double yMouse = 0.5;

// Bounds of viewing frustum.
float nearPlane =  1.0;
float farPlane  = 1000.0;

// Viewing angle.
double fovy = 40.0;

// Variables.
double alpha = 0;                                  // Set by idle function.
double beta = 0;                                   // Set by mouse X.
double mousedist = - (farPlane - nearPlane) / 2;    // Set by mouse Y.

LinkedStructure l;

Vector2f targetPoint = Vector2f::Zero();

// This function is called to display the scene.
void display ()
{
    glEnable(GL_LIGHTING);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Translate using Y mouse.
    //mousedist = - (yMouse * (farPlane - nearPlane) + nearPlane);
    glTranslatef(0, 0, -200);

    // Rotation from idle function.
    glRotatef(90, 0, 0, 1);

    // Rotation using X mouse.
    beta = 180.0 * xMouse;
    glRotatef(beta, 0, 1, 0);
    alpha = 180.0 * yMouse;
    glRotatef(beta, 1, 0, 0);

    l.draw();

    glPushMatrix();
    glColor3f(0.0f, 0.0f, 1.0f);
    glTranslatef(0, -targetPoint(1), targetPoint(0));
    glutSolidSphere(3.0f, 8, 8);
    glPopMatrix();
    
    glFlush();
    glutSwapBuffers();
}

// This function is called when there is nothing else to do.
void idle ()
{
    glutPostRedisplay();
}

// This function gets called every 10ms
void timer(int i)
{
  l.update();
  if(l.isTargetResolved()) {targetPoint = l.getPointWithinRange(); l.moveToPoint(targetPoint); }

    glutTimerFunc(10, timer, i);
    glutPostRedisplay();
}

void mouseMovement (int mx, int my)
{
    // Normalize mouse coordinates.
    xMouse = double(mx) / double(width);
    yMouse = 1 - double(my) / double(height);

    // Redisplay image.
    glutPostRedisplay();
}

// Respond to window resizing, preserving proportions.
// Parameters give new window size in pixels.
void reshapeMainWindow (int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, GLfloat(width) / GLfloat(height), nearPlane, farPlane);
}

// Respond to graphic character keys.
// Parameters give key code and mouse coordinates.
void graphicKeys (unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        exit(0);
    }
}

int main (int argc, char **argv)
{
    srand ( time(NULL) );

    // Constructing the linked structure by
    // adding links
    for (int i = 1; i <= 5; i++)
    {
      Color c = {1.0f, 1.0f, 1.0f, 1.0f};
        Link *l = new Link(c);
        l->mAngle = 3.14f/4;
        l->mLength = 20;
        ::l.addLink(l);
    }

    // GLUT initialization.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Inverse Kinematics Arm - by Alexandros Dermenakis");

    // Register call backs.
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeMainWindow);
    glutKeyboardFunc(graphicKeys);
    glutMotionFunc(mouseMovement);
    glutIdleFunc(idle);
    glutTimerFunc(60, timer, 0);

    // OpenGL initialization
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat global_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightModelfv(GL_AMBIENT_AND_DIFFUSE, global_ambient);

    // Enter GLUT loop.
    glutMainLoop();

    return 0;
}
