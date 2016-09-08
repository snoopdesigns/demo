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

// Terrain mesh size
#define L_RES 100
#define M_RES 700
#define H_RES 1500
#define N_MESH M_RES // Mesh size
#define N_CELLS (N_MESH-1) // Cells size
#define MESH_SCALE 5 // Mesh scale on [-1;1]

// Sky mesh size
#define N_SKY 200
#define N_SKY_CELLS (N_SKY-1)
#define N_SKY_SCALE (MESH_SCALE*4)

#define VERTEX_SEGMENT_LIMIT 100
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
GLint uniform_p;
GLint uniform_mvp;
GLint uniform_mvp_sky;
GLint uniform_camerapos;
GLint uniform_line_flag;
GLint uniform_sky_flag;
GLint uniform_scale;

GLuint texture_id;

bool rotate = false;

#define DEBUG_CAMERA true
float camera_x = -0.299998;
float camera_y = 1.749997;
float camera_z = 2.300000;
float lookat_x = 1.550001;
float lookat_y = 3.000000;
float lookat_z = 1.800000;
/*float camera_x = MESH_SCALE;
float camera_y = -MESH_SCALE;
float camera_z = 3.649999;
float lookat_x = MESH_SCALE;
float lookat_y = MESH_SCALE;
float lookat_z = 3.249999;*/
#define CAMERA_STEP 0.05
#define LOOK_STEP 0.05

// Mesh variables
GLuint vbo_sky[1];
GLuint vbo_sky_lines_index[1];
GLuint vbo_sky_triangles_index[1];

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
	uniform_p = get_uniform(program, "p");
	uniform_mvp = get_uniform(program, "mvp");
	uniform_mvp_sky = get_uniform(program, "mvp_sky");
	uniform_line_flag = get_uniform(program, "line_flag");
	uniform_sky_flag = get_uniform(program, "sky_flag");
	uniform_camerapos = get_uniform(program, "camerapos");
	uniform_randtexture = get_uniform(program, "randtexture");
	uniform_scale = get_uniform(program, "scale");
	
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
	
	glGenBuffers(1, vbo_sky);
	glGenBuffers(1, vbo_sky_lines_index);
	glGenBuffers(1, vbo_sky_triangles_index);

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
	
	// Sky mesh generation
	glm::vec2* skyVertices = new glm::vec2[N_SKY*N_SKY];
	generateVerticesMesh(skyVertices, N_SKY, N_SKY_SCALE, STRATEGY_CENTER);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sky[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * N_SKY*N_SKY, skyVertices, GL_STATIC_DRAW);
	
	GLushort* sky_tr_indices;
	GLushort* sky_line_indices;
	sky_tr_indices = new GLushort[N_SKY_CELLS * N_SKY_CELLS * 6];
	generateTrianglesIndices(sky_tr_indices, N_SKY_CELLS,N_SKY_CELLS);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sky_triangles_index[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * N_SKY_CELLS * N_SKY_CELLS * 6, sky_tr_indices, GL_STATIC_DRAW);
		
	sky_line_indices = new GLushort[N_SKY_CELLS * N_SKY_CELLS * 10];
	generateLinesIndices(sky_line_indices, N_SKY_CELLS, N_SKY_CELLS);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sky_lines_index[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * N_SKY_CELLS * N_SKY_CELLS * 10, sky_line_indices, GL_STATIC_DRAW);
	return 1;
}

void render(GLFWwindow* window) {

	glUseProgram(program);
	
	glUniform1i(uniform_randtexture, 0);
	glUniform1i(uniform_line_flag, 0);
	glUniform1i(uniform_scale, MESH_SCALE);
	
	glUniform3f(uniform_camerapos, camera_x, camera_y, camera_z);
	glEnableVertexAttribArray(attribute_coord2d);
	
	glUniform1i(uniform_sky_flag, 1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sky[0]);
	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sky_triangles_index[0]);
	glDrawElements(GL_TRIANGLES, N_SKY_CELLS * N_SKY_CELLS * 6, GL_UNSIGNED_SHORT, 0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sky_lines_index[0]);
	glDrawElements(GL_LINES, N_SKY_CELLS * N_SKY_CELLS * 10, GL_UNSIGNED_SHORT, 0);
	glUniform1i(uniform_sky_flag, 0);
	
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
  glDeleteBuffers(1, &vbo_sky[0]);
  glDeleteBuffers(1, &vbo_sky_lines_index[0]);
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
	glm::mat4 view_sky = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.5f * MESH_SCALE), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 projection = glm::perspective(45.0f, 1.0f * getMonitorWidth() / getMonitorHeight(), 0.01f, 500.0f);
	glm::mat4 mvp = projection * view * model;
	glm::mat4 mvp_sky = projection * view_sky * model;
	
	glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniform_p, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(uniform_mvp_sky, 1, GL_FALSE, glm::value_ptr(mvp_sky));
	
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
				gl_log("Rotation is now %s\n", rotate ? "on" : "off");
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
		if (DEBUG_CAMERA) {
			gl_log("#########################################\n");
			gl_log("float camera_x = %f;\n", camera_x);
			gl_log("float camera_y = %f;\n", camera_y);
			gl_log("float camera_z = %f;\n", camera_z);
			gl_log("float lookat_x = %f;\n", lookat_x);
			gl_log("float lookat_y = %f;\n", lookat_y);
			gl_log("float lookat_z = %f;\n", lookat_z);
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
