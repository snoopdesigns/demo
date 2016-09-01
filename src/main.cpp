// http://antongerdelan.net/opengl/glcontext2.html
// http://learnopengl.com/#!Getting-started/Hello-Triangle
// https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Introduction
// https://blenderartists.org/forum/archive/index.php/t-346766.html
// LINUX: gcc main.c -o main -lGL -lglut -lglfw -lGLEW
// WIN: gcc -Wall -o main -L/cygdrive/c/PROJECTS/demo/lib main.c -lopengl32 -lglfw3 -lfreeglut -lglew32
// NOISE TUTORIAL: http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=24

// TODO output text (camera position etc

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

#define N_MESH 200 // Mesh size
#define N_CELLS 199 // Cells size
#define MESH_SCALE 1 // MEsh scale on [-1;1]

#define DRAW_POLYGON_LINES true
#define DRAW_POLYGONS true

GLuint program;
GLint attribute_coord2d;
GLint uniform_vertex_transform;
GLint uniform_color;

bool rotate = false;

float camera_x = 0.0;
float camera_y = -2.0;
float camera_z = 1.0;
float lookat_x = 0.0;
float lookat_y = 0.0;
float lookat_z = 0.0;
#define CAMERA_STEP 0.05
#define LOOK_STEP 0.05

/*
  * 1st index is mesh vertices
  * 2nd index is vertices triangles indexes
  * 3rd index is lines indexes
*/
GLuint vbo[3];

int init_resources(void) {

	program = create_program("assets/shader.v.glsl", "assets/shader.f.glsl");
	if (program == 0)
		return 0;

	attribute_coord2d = get_attrib(program, "coord2d");
	uniform_vertex_transform = get_uniform(program, "vertex_transform");
	uniform_color = get_uniform(program, "color");

	// Create three vertex buffer objects
	glGenBuffers(3, vbo);

	// Create an array for vertices
	glm::vec2 vertices[N_MESH*N_MESH];
	generateVerticesMesh(vertices, N_MESH, MESH_SCALE);

	// Tell OpenGL to copy our array to the buffer objects
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

	// Create an array of triangles indices
	if (DRAW_POLYGONS) {
		GLushort indices[N_CELLS * N_CELLS * 6];
		generateTrianglesIndices(indices, N_CELLS);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);
	}
	
	// Create an array of lines indices
	if (DRAW_POLYGON_LINES) {
		GLushort linesIndices[N_CELLS * N_CELLS * 10];
		generateLinesIndices(linesIndices, N_CELLS);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof linesIndices, linesIndices, GL_STATIC_DRAW);
    }
	return 1;
}

void render(GLFWwindow* window) {

	glUseProgram(program);
	
	GLfloat white[4] = {1, 1, 1, 1};
    glUniform4fv(uniform_color, 1, white);
	glEnableVertexAttribArray(attribute_coord2d);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);

	if (DRAW_POLYGONS) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
		glDrawElements(GL_TRIANGLES, N_CELLS * N_CELLS * 6, GL_UNSIGNED_SHORT, 0);
	}
	
	if (DRAW_POLYGON_LINES) {
		GLfloat bright[4] = {1.2, 1.2, 1.2, 1};
		glUniform4fv(uniform_color, 1, bright);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
		glDrawElements(GL_LINES, N_CELLS * N_CELLS * 10, GL_UNSIGNED_SHORT, 0);
	}

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
	glm::mat4 view = glm::lookAt(glm::vec3(camera_x, camera_y, camera_z), glm::vec3(lookat_x, lookat_y, lookat_z), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 projection = glm::perspective(45.0f, 1.0f * getMonitorWidth() / getMonitorHeight(), 0.01f, 50.0f);
	glm::mat4 vertex_transform = projection * view * model;
	glUniformMatrix4fv(uniform_vertex_transform, 1, GL_FALSE, glm::value_ptr(vertex_transform));
}

void mainLoop(GLFWwindow* window) {
	do {
		glClearColor(1.0, 1.0, 1.0, 1.0);
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
			case GLFW_KEY_F3:
				rotate = !rotate;
				printf("Rotation is now %s\n", rotate ? "on" : "off");
				break;
			case GLFW_KEY_UP:
				camera_y += CAMERA_STEP;
				break;
			case GLFW_KEY_DOWN:
				camera_y += -1.0 * CAMERA_STEP;
				break;
			case GLFW_KEY_LEFT:
				camera_x += -1.0 * CAMERA_STEP;
				break;
			case GLFW_KEY_RIGHT:
				camera_x += CAMERA_STEP;
				break;
			case GLFW_KEY_PAGE_UP:
				camera_z += CAMERA_STEP;
				break;
			case GLFW_KEY_PAGE_DOWN:
				camera_z += -1.0 * CAMERA_STEP;
				break;
			case GLFW_KEY_KP_8:
				lookat_z += LOOK_STEP;
				break;
			case GLFW_KEY_KP_2:
				lookat_z += -1.0 * LOOK_STEP;
				break;
			case GLFW_KEY_KP_4:
				lookat_x += -1.0 * LOOK_STEP;
				break;
			case GLFW_KEY_KP_6:
				lookat_x += LOOK_STEP;
				break;
			case GLFW_KEY_KP_1:
				lookat_y += LOOK_STEP;
				break;
			case GLFW_KEY_KP_3:
				lookat_y += -1.0 * LOOK_STEP;
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
	int res = init_resources();
	if(res != 1) {
	    return 0;
	}
	glfwSetKeyCallback(window, key_callback);
	mainLoop(window);
	free_resources();
  return 0;
}
