#ifndef _SHADER_H
#define _SHADER_H

#define GLEW_NO_GLU 1

#include <GL/glew.h>
#include "log.h"
#include "utils.h"

extern void print_shader_error_log(GLuint object);
extern GLuint create_shader(const char* filename, GLenum type);
extern GLint get_attrib(GLuint program, const char *name);
extern GLint get_uniform(GLuint program, const char *name);
extern GLuint create_program(const char *vertexfile, const char *fragmentfile);

#endif