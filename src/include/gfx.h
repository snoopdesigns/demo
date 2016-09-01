#ifndef _GFX_H
#define _GFX_H

#define GLEW_NO_GLU 1

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "log.h"

extern GLFWwindow* initializeWindow(void);
extern int getMonitorWidth();
extern int getMonitorHeight();

#endif