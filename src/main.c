// http://antongerdelan.net/opengl/glcontext2.html
// http://learnopengl.com/#!Getting-started/Hello-Triangle
// https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Introduction
// LINUX: gcc main.c -o main -lGL -lglut -lglfw -lGLEW
// WIN: gcc -Wall -o main -L/cygdrive/c/PROJECTS/demo/lib main.c -lopengl32 -lglfw3 -lfreeglut -lglew32

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "include/log.h"
#include "include/gfx.h"

// Include GL headers
#include <GL/gl.h>

GLuint program;
GLint attribute_coord2d;

int init_resources(void) {
    GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	const char *vs_source =
		"#version 120\n"  // OpenGL 2.1
		"attribute vec2 coord2d;                  "
		"void main(void) {                        "
		"  gl_Position = vec4(coord2d, 0.0, 1.0); "
		"}";
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);
	if (!compile_ok) {
		gl_log("Error in vertex shader");
		return 0;
	}
	
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	const char *fs_source =
		"#version 120\n"  // OpenGL 2.1
		"void main(void) {        "
		"  gl_FragColor[0] = gl_FragCoord.x/640.0; "
		"  gl_FragColor[1] = gl_FragCoord.y/480.0; "
		"  gl_FragColor[2] = 0.5; "
		"}";
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
	if (!compile_ok) {
		gl_log("Error in fragment shader");
		return 0;
	}
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		gl_log("Error in glLinkProgram");
		return 0;
	}
	const char* attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord2d == -1) {
		gl_log("Could not bind attribute %s", attribute_name);
		return 0;
	}
  return 1;
}

void render(GLFWwindow* window) {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord2d);
	GLfloat triangle_vertices[] = {
	    0.0,  0.8,
	   -0.8, -0.8,
	    0.8, -0.8,
	};
	/* Describe our vertices array to OpenGL (it can't guess its format automatically) */
	glVertexAttribPointer(
		attribute_coord2d, // attribute
		2,                 // number of elements per vertex, here (x,y)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		triangle_vertices  // pointer to the C array
						  );
	
	/* Push each element in buffer_vertices to the vertex shader */
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(attribute_coord2d);
}

void free_resources() {
  glDeleteProgram(program);
}

void mainLoop(GLFWwindow* window) {
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render(window);
		glfwPollEvents();
		glfwSwapBuffers(window);
	} while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
}

int main(int argc, char **argv)
{
	GLFWwindow* window = initializeWindow();
	if (window == NULL) {
	    return 0;
	}
	if(!init_resources()) {
	    return 0;
	}
	mainLoop(window);
	free_resources();
  return 0;
}