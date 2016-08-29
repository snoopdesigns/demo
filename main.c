#include <GL/gl.h>
#include <GL/glu.h>
#include "include/glut.h"

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Here to draw
    glutSwapBuffers();
}

void initGfx(void)
{
    // Here to init graphics
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutCreateWindow("My first demo");
  glutDisplayFunc(display);
  initGfx();
  glutMainLoop();
  return 0;
}