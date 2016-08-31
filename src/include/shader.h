#ifndef _SHADER_H
#define _SHADER_H

#define GLEW_NO_GLU 1

#include <GL/glew.h>
#include "log.h"
#include "utils.h"

extern void print_shader_error_log(GLuint object);
extern GLuint create_shader(const char* filename, GLenum type);

#endif