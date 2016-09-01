// http://antongerdelan.net/opengl/glcontext2.html
// http://learnopengl.com/#!Getting-started/Hello-Triangle
// https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Introduction
// LINUX: gcc main.c -o main -lGL -lglut -lglfw -lGLEW
// WIN: gcc -Wall -o main -L/cygdrive/c/PROJECTS/demo/lib main.c -lopengl32 -lglfw3 -lfreeglut -lglew32
// NOISE TUTORIAL: http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=24

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "include/log.h"
#include "include/gfx.h"
#include "include/utils.h"
#include "include/shader.h"

//#define GLM_FORCE_RADIANS // remove compile warnings
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Include GL headers
#include <GL/gl.h>

#define N_TEXTURE 256 // Texture size
#define N_MESH 100 // Mesh size

GLuint program;
GLint attribute_coord2d;
GLint uniform_vertex_transform;
GLuint texture_id;
GLint uniform_mytexture;
GLint uniform_color;

bool interpolate = false;
bool rotate = false;

/*
  * 1st index is mesh vertices
  * 2nd index is vertices triangles indexes
  * 3rd index is lines indexes
*/
GLuint vbo[3];

int init_resources(void) {

	program = create_program("assets/triangle.v.glsl", "assets/triangle.f.glsl");
	if (program == 0)
		return 0;

	attribute_coord2d = get_attrib(program, "coord2d");
	uniform_vertex_transform = get_uniform(program, "vertex_transform");
	uniform_mytexture = get_uniform(program, "mytexture");
	uniform_color = get_uniform(program, "color");
	
	GLbyte graph[N_TEXTURE*N_TEXTURE];
	generateTexture(graph, N_TEXTURE);

	/* Upload the texture with our datapoints */
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, N_TEXTURE, N_TEXTURE, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, graph);

	// Create three vertex buffer objects
	glGenBuffers(3, vbo);

	// Create an array for vertices
	glm::vec2 vertices[(N_MESH + 1)*(N_MESH + 1)];
	generateVerticesMesh(vertices, N_MESH + 1);

	// Tell OpenGL to copy our array to the buffer objects
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

	// Create an array of triangles indices
	GLushort indices[N_MESH * N_MESH * 6];
	generateTrianglesIndices(indices, N_MESH);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);
	
	// Create an array of lines шndices
	GLushort linesIndices[N_MESH * N_MESH * 10];
	generateLinesIndices(linesIndices, N_MESH);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof linesIndices, linesIndices, GL_STATIC_DRAW);
  return 1;
}

void render(GLFWwindow* window) {

	glUseProgram(program);
	glUniform1i(uniform_mytexture, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolate ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolate ? GL_LINEAR : GL_NEAREST);
	
	GLfloat white[4] = {1, 1, 1, 1};
    glUniform4fv(uniform_color, 1, white);
	glEnableVertexAttribArray(attribute_coord2d);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glDrawElements(GL_TRIANGLES, N_MESH * N_MESH * 6, GL_UNSIGNED_SHORT, 0);
	
	GLfloat bright[4] = {2, 2, 2, 1};
	glUniform4fv(uniform_color, 1, bright);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glDrawElements(GL_LINES, N_MESH * N_MESH * 10, GL_UNSIGNED_SHORT, 0);

	/* Stop using the vertex buffer object */
	glDisableVertexAttribArray(attribute_coord2d);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void free_resources() {
  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo[0]);
  glDeleteBuffers(1, &vbo[1]);
  glDeleteBuffers(1, &vbo[2]);
}

void logic() {
	glm::mat4 model;

	if (rotate) {
		model = glm::rotate(glm::mat4(1.0f), glm::radians(clock() / 100.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	} else {
		model = glm::mat4(1.0f);
	}
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, -2.0, 2.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 projection = glm::perspective(45.0f, 1.0f * 640 / 480, 0.1f, 10.0f);
	glm::mat4 vertex_transform = projection * view * model;
	glUniformMatrix4fv(uniform_vertex_transform, 1, GL_FALSE, glm::value_ptr(vertex_transform));
}

void mainLoop(GLFWwindow* window) {
	do {
		glClearColor(0.8, 0.8, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		logic();
		render(window);
		glfwPollEvents();
		glfwSwapBuffers(window);
	} while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_F4:
				interpolate = !interpolate;
				printf("Interpolation is now %s\n", interpolate ? "on" : "off");
				break;
			case GLFW_KEY_F3:
				rotate = !rotate;
				printf("Rotation is now %s\n", rotate ? "on" : "off");
				break;
		}
	}
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
	glfwSetKeyCallback(window, key_callback);
	mainLoop(window);
	free_resources();
  return 0;
}
