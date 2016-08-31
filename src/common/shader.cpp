#include "../include/shader.h"

/**
 * Display compilation errors from the OpenGL shader compiler
 */
void print_shader_error_log(GLuint object) {
	GLint log_length = 0;
	if (glIsShader(object)) {
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	} else if (glIsProgram(object)) {
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	} else {
		gl_log("printlog: Not a shader or a program");
		return;
	}

	char* log = (char*)malloc(log_length);
	
	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, log);
	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, log);
	
	gl_log(log);
	free(log);
}

/**
 * Compile the shader from file 'filename', with error handling
 */
GLuint create_shader(const char* filename, GLenum type) {
	const GLchar* source = read_from_file(filename);
	if (source == NULL) {
		gl_log("Error opening %s", filename);
		return 0;
	}
	GLuint res = glCreateShader(type);
	const GLchar* sources[] = {"#version 120\n",source};
	gl_log("Loading shader: \n%s\n", source);
	glShaderSource(res, 2, sources, NULL);
	free((void*)source);
	
	glCompileShader(res);
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) {
		gl_log("%s:",filename);
		print_shader_error_log(res);
		glDeleteShader(res);
		return 0;
	}
	
	return res;
}