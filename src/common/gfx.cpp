#include "../include/gfx.h"

void glfw_error_callback(int error, const char* description) {
    gl_log_err("GLFW ERROR: code %i msg: %s\n", error, description);
}

int getMonitorWidth() {
    GLFWmonitor* mon = glfwGetPrimaryMonitor();
	const GLFWvidmode* vmode = glfwGetVideoMode(mon); 
	return vmode->width * 0.9;
}

int getMonitorHeight() {
    GLFWmonitor* mon = glfwGetPrimaryMonitor();
	const GLFWvidmode* vmode = glfwGetVideoMode(mon); 
	return vmode->height * 0.9;
}

GLFWwindow* initializeWindow(void) {
    if (!restart_gl_log()) {
	    fprintf(stderr, "Failed to restart GLFW log\n");
		return NULL;
	}
	gl_log("Starting GLFW...\n");
	gl_log("GLFW version: %s\n", glfwGetVersionString());
	glfwSetErrorCallback(glfw_error_callback);
	if(!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* mon = glfwGetPrimaryMonitor();
	const GLFWvidmode* vmode = glfwGetVideoMode(mon);
	gl_log("width=%d\n", vmode->width);
	gl_log("height=%d\n", vmode->height);
	//window = glfwCreateWindow (vmode->width, vmode->height, "My first demo", mon, NULL);
	GLFWwindow* window = glfwCreateWindow (getMonitorWidth(), getMonitorHeight(), "My first demo", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return NULL;
	}
	//glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
	//glCullFace(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPolygonOffset(1, 1);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	return window;
}
