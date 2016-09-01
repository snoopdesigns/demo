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
#include <math.h>
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

GLuint program;
GLint attribute_coord2d;
GLint uniform_vertex_transform;
GLuint texture_id;
GLint uniform_mytexture;

bool interpolate = false;
bool rotate = false;

GLuint vbo[2];

int init_resources(void) {

	program = create_program("assets/triangle.v.glsl", "assets/triangle.f.glsl");
	if (program == 0)
		return 0;

	attribute_coord2d = get_attrib(program, "coord2d");
	uniform_vertex_transform = get_uniform(program, "vertex_transform");
	uniform_mytexture = get_uniform(program, "mytexture");

	if (attribute_coord2d == -1 || uniform_vertex_transform == -1 || uniform_mytexture == -1)
		return 0;

	// Create our datapoints, store it as bytes
#define N_TEXTURE 256
	GLbyte graph[N_TEXTURE][N_TEXTURE];

	for (int i = 0; i < N_TEXTURE; i++) {
		for (int j = 0; j < N_TEXTURE; j++) {
			float x = (i - N_TEXTURE / 2) / (N_TEXTURE / 2.0);
			float y = (j - N_TEXTURE / 2) / (N_TEXTURE / 2.0);
			float d = hypotf(x, y) * 4.0;
			float z = (1 - d * d) * expf(d * d / -2.0);

			graph[i][j] = roundf(z * 127 + 128);
		}
	}

	/* Upload the texture with our datapoints */
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	gl_log("Texture ID: %d",texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, N_TEXTURE, N_TEXTURE, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, graph);

	// Create two vertex buffer objects
	glGenBuffers(2, vbo);

	// Create an array for 101 * 101 vertices
#define N_MESH 100
#define N_MESH_DIV_TWO 50
#define N_MESH_INC 101
	glm::vec2 vertices[N_MESH_INC][N_MESH_INC];

	for (int i = 0; i < N_MESH_INC; i++) {
		for (int j = 0; j < N_MESH_INC; j++) {
			vertices[i][j].x = (j - N_MESH_DIV_TWO) / 50.0; // this is N_MESH_DIV_TWO
			vertices[i][j].y = (i - N_MESH_DIV_TWO) / 50.0;
		}
	}

	// Tell OpenGL to copy our array to the buffer objects
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

	// Create an array of indices into the vertex array that traces both horizontal and vertical lines
	GLushort indices[N_MESH * N_MESH * 6];
	int i = 0;

	for (int x = 0; x < N_MESH; x++) {
		for (int y = 0; y < N_MESH; y++) {
			indices[i++] = x * N_MESH_INC + y;
			indices[i++] = (x + 1) * N_MESH_INC + y;
			indices[i++] = (x + 1) * N_MESH_INC + y + 1;
			indices[i++] = (x + 1) * N_MESH_INC + y + 1;
			indices[i++] = x * N_MESH_INC + y + 1;
			indices[i++] = x * N_MESH_INC + y;
		}
	}
	
	for(int i=0;i<N_MESH * N_MESH_INC * 6;i++) {
	    printf("%d ", indices[i]);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);
  return 1;
}

void render(GLFWwindow* window) {

	glUseProgram(program);
	glUniform1i(uniform_mytexture, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolate ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolate ? GL_LINEAR : GL_NEAREST);

	/* Draw the grid using the indices to our vertices using our vertex buffer objects */
	glEnableVertexAttribArray(attribute_coord2d);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glDrawElements(GL_TRIANGLES, 100 * 101 * 6, GL_UNSIGNED_SHORT, 0);

	/* Stop using the vertex buffer object */
	glDisableVertexAttribArray(attribute_coord2d);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void free_resources() {
  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo[0]);
  glDeleteBuffers(1, &vbo[1]);
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
