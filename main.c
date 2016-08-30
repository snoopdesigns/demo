// http://antongerdelan.net/opengl/glcontext2.html
// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#define GLEW_NO_GLU 1
#define GLFW_INCLUDE_GLCOREARB
// Include GL headers
#include <GL/glew.h>

#include <GL/gl.h>
#include "include/glfw3.h"

#include "log.c"

GLFWwindow* window;

float points[] = {
  -0.5f,  0.5f,  0.0f,
   0.5f,  0.5f,  0.0f,
  -0.5f, -0.5f,  0.0f,
   0.5f, -0.5f,  0.0f,
   0.5f,  0.5f,  0.0f,
  -0.5f, -0.5f,  0.0f,
};
const char* vertex_shader =
	"#version 400\n"
	"in vec3 vp;"
	"void main () {"
	"  gl_Position = vec4 (vp.x, vp.y, vp.z, 1.0);"
	"}";
const char* fragment_shader =
	"#version 400\n"
	"out vec4 frag_colour;"
	"void main () {"
	"  frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
	"}";

void glfw_error_callback(int error, const char* description) {
    gl_log_err("GLFW ERROR: code %i msg: %s\n", error, description);
}
	
int initializeGraphics(void) {
    if (!restart_gl_log()) {
	    fprintf( stderr, "Failed to restart GLFW log\n" );
		return -1;
	}
	gl_log("Starting GLFW...\n");
	gl_log("GLFW version: %s\n", glfwGetVersionString());
	glfwSetErrorCallback(glfw_error_callback);
	if(!glfwInit())
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

	GLFWmonitor* mon = glfwGetPrimaryMonitor ();
	const GLFWvidmode* vmode = glfwGetVideoMode(mon);
	GLFWwindow* window = glfwCreateWindow (vmode->width, vmode->height, "My first demo", mon, NULL);
	
	/*window = glfwCreateWindow(1024, 768, "Tutorial 01", NULL, NULL);
	if(window == NULL){
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}*/
	glfwMakeContextCurrent(window);
	
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
	const GLubyte* renderer = glGetString (GL_RENDERER);
	const GLubyte* version = glGetString (GL_VERSION);
	printf ("Renderer: %s\n", renderer);
	printf ("OpenGL version supported %s\n", version);
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);
	return 1;
}

int draw(void) {
    GLuint vbo = 0;
	glGenBuffers (1, &vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, 18 * sizeof (float), points, GL_STATIC_DRAW);
	GLuint vao = 0;
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	GLuint vs = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource (vs, 1, &vertex_shader, NULL);
	glCompileShader (vs);
	GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource (fs, 1, &fragment_shader, NULL);
	glCompileShader (fs);
	GLuint shader_programme = glCreateProgram ();
	glAttachShader (shader_programme, fs);
	glAttachShader (shader_programme, vs);
	glLinkProgram (shader_programme);
	
	glUseProgram (shader_programme);
	glBindVertexArray (vao);
	glDrawArrays (GL_TRIANGLES , 0, 6);
	return 1;
}

int main(int argc, char **argv)
{
	if (initializeGraphics() != 1) {
	    return 0;
	}
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	
	do {
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw();
		glfwPollEvents ();
		glfwSwapBuffers (window);
	}
	while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
  return 0;
}
