// http://antongerdelan.net/opengl/glcontext2.html
// http://learnopengl.com/#!Getting-started/Hello-Triangle
// https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Introduction
// https://blenderartists.org/forum/archive/index.php/t-346766.html
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/
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

#define N_MESH 900 // Mesh size
#define N_CELLS (N_MESH-1) // Cells size
#define MESH_SCALE 7 // Mesh scale on [-1;1]

#define VERTEX_SEGMENT_LIMIT 15
#define VERTEX_SEGMENT_SIZE_LIMIT 256*256

#define DRAW_POLYGON_LINES false
#define DRAW_POLYGONS true

#define STRATEGY_CENTER 0
#define STRATEGY_Q1 1

#define MESH_GENERATION_STRATEGY STRATEGY_Q1

// Shader program
GLuint program;

// Shader attributes
GLint attribute_coord2d;

// Shader uniforms
GLint uniform_randtexture;
GLint uniform_m;
GLint uniform_v;
GLint uniform_mvp;
GLint uniform_camerapos;
GLint uniform_line_flag;

GLuint texture_id;

bool rotate = false;

float camera_x = 5.750006;
float camera_y = -2.349980;
float camera_z = 4.899995;
float lookat_x = MESH_SCALE/2.0;
float lookat_y = MESH_SCALE/2.0;
float lookat_z = 2.0;
#define CAMERA_STEP 0.05
#define LOOK_STEP 0.05

// Mesh variables
GLuint vbo_vertex[VERTEX_SEGMENT_LIMIT];
GLuint vbo_triangles_index[VERTEX_SEGMENT_LIMIT];
GLuint vbo_lines_index[VERTEX_SEGMENT_LIMIT];
int num_segments;
int* segment_sizes = new int[VERTEX_SEGMENT_LIMIT];

int init_resources(void) {

	program = create_program("assets/shader.v.glsl", "assets/shader.f.glsl");
	if (program == 0)
		return 0;

	attribute_coord2d = get_attrib(program, "coord2d");
	uniform_m = get_uniform(program, "m");
	uniform_v = get_uniform(program, "v");
	uniform_mvp = get_uniform(program, "mvp");
	uniform_line_flag = get_uniform(program, "line_flag");
	uniform_camerapos = get_uniform(program, "camerapos");
	uniform_randtexture = get_uniform(program, "randtexture");
	
	// Generate random texture
	#define N 256
	GLbyte* graph = new GLbyte[N*N];
	generateTexture(graph, N);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, N, N, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, graph);

	// Create three vertex buffer objects
	glGenBuffers(VERTEX_SEGMENT_LIMIT, vbo_vertex);
	glGenBuffers(VERTEX_SEGMENT_LIMIT, vbo_triangles_index);
	glGenBuffers(VERTEX_SEGMENT_LIMIT, vbo_lines_index);

	// Create an array for vertices
	glm::vec2* vertices = new glm::vec2[N_MESH*N_MESH];
	glm::vec2* verticesSplitted = new glm::vec2[VERTEX_SEGMENT_SIZE_LIMIT*VERTEX_SEGMENT_LIMIT];
	generateVerticesMesh(vertices, N_MESH, MESH_SCALE, MESH_GENERATION_STRATEGY);
	num_segments = splitVerticesMesh(vertices, N_MESH, VERTEX_SEGMENT_SIZE_LIMIT, verticesSplitted, segment_sizes);
	GLushort* indices;
	GLushort* linesIndices;
	for(int segment_i = 0; segment_i < num_segments; segment_i++){
		// Binding vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex[segment_i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * (segment_sizes[segment_i])*N_MESH, &verticesSplitted[segment_i*VERTEX_SEGMENT_SIZE_LIMIT], GL_STATIC_DRAW);
		
		// Binding triangles index buffer
		indices = new GLushort[(segment_sizes[segment_i]-1) * N_CELLS * 6];
		generateTrianglesIndices(indices, N_CELLS,(segment_sizes[segment_i]-1));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_triangles_index[segment_i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * (segment_sizes[segment_i]-1) * N_CELLS * 6, indices, GL_STATIC_DRAW);
		
		// Binding lines index buffer
		linesIndices = new GLushort[(segment_sizes[segment_i]-1) * N_CELLS * 10];
		generateLinesIndices(linesIndices, N_CELLS, (segment_sizes[segment_i]-1));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_lines_index[segment_i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * (segment_sizes[segment_i]-1) * N_CELLS * 10, linesIndices, GL_STATIC_DRAW);
	}
	return 1;
}

void render(GLFWwindow* window) {

	glUseProgram(program);
	
	glUniform1i(uniform_randtexture, 0);
	glUniform1i(uniform_line_flag, 0);
	
	glUniform3f(uniform_camerapos, camera_x, camera_y, camera_z);
	glEnableVertexAttribArray(attribute_coord2d);
	for(int segment_i = 0; segment_i < num_segments; segment_i++){
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex[segment_i]);
		glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
		
		if (DRAW_POLYGONS) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_triangles_index[segment_i]);
			glDrawElements(GL_TRIANGLES, (segment_sizes[segment_i]-1) * N_CELLS * 6, GL_UNSIGNED_SHORT, 0);
		}
		
		if (DRAW_POLYGON_LINES) {
			glUniform1i(uniform_line_flag, 1);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_lines_index[segment_i]);
			glDrawElements(GL_LINES, (segment_sizes[segment_i]-1) * N_CELLS * 10, GL_UNSIGNED_SHORT, 0);
		}
	}
	/* Stop using the vertex buffer object */
	glDisableVertexAttribArray(attribute_coord2d);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void free_resources() {
  glDeleteProgram(program);
  for(int i = 0;i < VERTEX_SEGMENT_LIMIT; i++) {
	glDeleteBuffers(1, &vbo_vertex[i]);
	glDeleteBuffers(1, &vbo_triangles_index[i]);
	glDeleteBuffers(1, &vbo_lines_index[i]);
  }
  
}

void logic() {
	glm::mat4 model;

	if (rotate) {
		model = glm::rotate(glm::mat4(1.0f), glm::radians(clock() / 100.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	} else {
		model = glm::mat4(1.0f);
	}
	glm::mat4 view = glm::lookAt(glm::vec3(camera_x, camera_y, camera_z), glm::vec3(lookat_x, lookat_y, lookat_z), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 projection = glm::perspective(45.0f, 1.0f * getMonitorWidth() / getMonitorHeight(), 0.01f, 500.0f);
	glm::mat4 mvp = projection * view * model;
	glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	
	/* Set texture wrapping mode */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, false ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, false ? GL_CLAMP_TO_EDGE : GL_REPEAT);

	/* Set texture interpolation mode */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, false ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, false ? GL_LINEAR : GL_NEAREST);
}

void mainLoop(GLFWwindow* window) {
	do {
		//glClearColor(0.55,0.65,0.75,1.0); // blue
		glClearColor(1.0, 1.0, 1.0, 1.0);
		//glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		logic();
		render(window);
		glfwPollEvents();
		glfwSwapBuffers(window);
	} while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_REPEAT || action == GLFW_PRESS) {
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
		//gl_log("campos[%f, %f, %f], camlooat[%f, %f, %f,]", camera_x, camera_y, camera_z, lookat_x, lookat_y, lookat_z);
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
