#include "../include/utils.h"
#include "../include/log.h"

#define DEBUG false

char* read_from_file(const char *filename)
{
    long int size = 0;
    FILE *file = fopen(filename, "r");

    if(!file) {
        gl_log("File open error\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);

    char *result = (char *) malloc(size+1);
    if(!result) {
        gl_log("Memory error\n");
        return NULL;
    }

    if(fread(result, 1, size, file) != size) {
        gl_log("Read file error\n");
        return NULL;
    }
	result[size] = '\0';

    fclose(file);
    return result;
}

void generateTexture(GLbyte* graph, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			float x = (i - size / 2) / (size / 2.0);
			float y = (j - size / 2) / (size / 2.0);
			float d = hypotf(x, y) * 4.0;
			float z = (1 - d * d) * expf(d * d / -2.0);
			graph[i*size+j] = roundf(z * 127 + 128);
			graph[i*size+j] = roundf((float)rand()/(float)(RAND_MAX) * 127 + 128);
			//printf("%f ", roundf((float)rand()/(float)(RAND_MAX) * 127 + 128));
		}
	}
}

void generateVerticesMesh(glm::vec2* vertices, int size, int scale) {
	float ratio_x = (2.0 * scale) / (size-1);
	float ratio_y = (2.0 * scale) / (size-1);
	
	
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			vertices[i*size+j].x = (-1.0 * scale) + j * ratio_x;
			vertices[i*size+j].y = (1.0 * scale) - i * ratio_y;
			if(DEBUG) gl_log("Vertex generated: [%f, %f]\n", vertices[i*size+j].x, vertices[i*size+j].y);
		}
	}
}

glm::vec3 calculateSurfaceNormal(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) {
	glm::vec3 normal = glm::vec3(0.0f);
	glm::vec2 U = p2 - p1;
	glm::vec2 V = p3 - p1;

	normal[0] = (U[1] * V[2]) - (U[2] * V[1]);
	normal[1] = (U[2] * V[0]) - (U[0] * V[2]);
	normal[2] = (U[0] * V[1]) - (U[1] * V[0]);

	return normal;
}

void generateTrianglesIndices(GLushort* indices, int size) {
    int i = 0;
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			indices[i++] = x * (size + 1) + y;
			indices[i++] = (x + 1) * (size + 1) + y;
			indices[i++] = (x + 1) * (size + 1) + y + 1;
			indices[i++] = (x + 1) * (size + 1) + y + 1;
			indices[i++] = x * (size + 1) + y + 1;
			indices[i++] = x * (size + 1) + y;
			if(DEBUG) gl_log("Triangle index: [%d,%d,%d,%d,%d,%d]\n", indices[i-6],indices[i-5],indices[i-4],indices[i-3],indices[i-2],indices[i-1]);
		}
	}
}

void generateLinesIndices(GLushort* indices, int size) {
    int i = 0;
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			indices[i++] = x * (size+1) + y;
			indices[i++] = (x + 1) * (size+1) + y;
			indices[i++] = (x + 1) * (size+1) + y;
			indices[i++] = (x + 1) * (size+1) + y + 1;
			indices[i++] = (x + 1) * (size+1) + y + 1;
			indices[i++] = x * (size+1) + y + 1;
			indices[i++] = x * (size+1) + y + 1;
			indices[i++] = x * (size+1) + y;
			indices[i++] = x * (size+1) + y;
			indices[i++] = (x + 1) * (size+1) + y + 1;
			if(DEBUG) gl_log("Line index: [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\n", indices[i-10],indices[i-9],indices[i-8],indices[i-7],indices[i-6],indices[i-5],indices[i-4],indices[i-3],indices[i-2],indices[i-1]);
		}
	}
}
