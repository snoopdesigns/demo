#include "../include/log.h"

#define GL_LOG_FILE "gl.log"

int restart_gl_log() {
	FILE* file = fopen(GL_LOG_FILE, "w");
	if (!file) {
	    fprintf(stderr, "ERROR: could not open GL_LOG_FILE log file %s for writing\n", GL_LOG_FILE);
		return 0;
	}
	time_t now = time(NULL);
	char* date = ctime(&now);
	fprintf(file, "GL_LOG_FILE log. local time %s\n", date);
	fclose(file);
	return 1;
}
	
int gl_log(const char* message, ...) {
	va_list argptr;
	FILE* file = fopen(GL_LOG_FILE, "a");
	if (!file) {
		fprintf(stderr, "ERROR: could not open GL_LOG_FILE %s file for appending\n", GL_LOG_FILE);
		return 0;
	}
	va_start(argptr, message);
	vfprintf(file, message, argptr);
	va_end(argptr);
	fclose(file);
	return 1;
}

int gl_log_err (const char* message, ...) {
    va_list argptr;
    FILE* file = fopen(GL_LOG_FILE, "a");
    if (!file) {
        fprintf(stderr, "ERROR: could not open GL_LOG_FILE %s file for appending\n", GL_LOG_FILE);
        return 0;
    }
    va_start(argptr, message);
    vfprintf(file, message, argptr);
    va_end(argptr);
    va_start(argptr, message);
    vfprintf(stderr, message, argptr);
    va_end(argptr);
    fclose(file);
    return 1;
}
