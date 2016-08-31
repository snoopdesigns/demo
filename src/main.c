// http://antongerdelan.net/opengl/glcontext2.html
// http://learnopengl.com/#!Getting-started/Hello-Triangle
// https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Introduction
// LINUX: gcc main.c -o main -lGL -lglut -lglfw -lGLEW
// WIN: gcc -Wall -o main -L/cygdrive/c/PROJECTS/demo/lib main.c -lopengl32 -lglfw3 -lfreeglut -lglew32

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>

#include "include/log.h"
#include "include/gfx.h"
#include "include/utils.h"
#include "include/shader.h"

// Include GL headers
#include <GL/gl.h>

GLuint program;
GLuint vbo_triangle;
GLint attribute_coord2d;
GLint uniform_fade;

int init_resources(void) {

	// Creating VBO to store vertices to graphic card
	GLfloat triangle_vertices[] = {
	    0.0,  0.8,
	   -0.8, -0.8,
	    0.8, -0.8,
	};
	glGenBuffers(1, &vbo_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	// Store vertices in memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

    GLuint vs, fs;
	GLint link_ok = GL_FALSE;
	if ((vs = create_shader("assets/triangle.v.glsl", GL_VERTEX_SHADER))   == 0) return 0;
	if ((fs = create_shader("assets/triangle.f.glsl", GL_FRAGMENT_SHADER)) == 0) return 0;
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		gl_log("Error in glLinkProgram");
		print_shader_error_log(program);
		return 0;
	}
	const char* attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord2d == -1) {
		gl_log("Could not bind attribute %s", attribute_name);
		return 0;
	}
	
	const char* uniform_name;
	uniform_name = "fade";
	uniform_fade = glGetUniformLocation(program, uniform_name);
	if (uniform_fade == -1) {
		gl_log("Could not bind uniform_fade %s", uniform_name);
		return 0;
	}
	glUniform1f(uniform_fade, 0.1);
  return 1;
}

void render(GLFWwindow* window) {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	glEnableVertexAttribArray(attribute_coord2d);
	
	/* Describe our vertices array to OpenGL (it can't guess its format automatically) */
	glVertexAttribPointer(
		attribute_coord2d, // attribute
		2,                 // number of elements per vertex, here (x,y)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of the first element
	);
	
	/* Push each element in buffer_vertices to the vertex shader */
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(attribute_coord2d);
}

void free_resources() {
  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo_triangle);
}

void logic() {
	float cur_fade = sinf(clock() / 1000.0 * (2*3.14) / 5) / 2 + 0.5;
	glUseProgram(program);
	glUniform1f(uniform_fade, cur_fade);
}

void mainLoop(GLFWwindow* window) {
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		logic();
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
