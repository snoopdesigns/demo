#include "../include/utils.h"

char *read_from_file(const char *filename)
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

    char *result = (char *) malloc(size);
    if(!result) {
        gl_log("Memory error\n");
        return NULL;
    }

    if(fread(result, 1, size, file) != size) {
        gl_log("Read file error\n");
        return NULL;
    }

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
		}
	}
}

void generateVerticesMesh(glm::vec2* vertices, int size, int scale) {
	float divider = size/(2 * scale);
	
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			vertices[i*size+j].x = (j - (size/2)) / divider;
			vertices[i*size+j].y = (i - (size/2)) / divider;
		}
	}
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
		}
	}
}

void generateLinesIndices(GLushort* indices, int size) {
    int i = 0;
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			indices[i++] = x * (size + 1) + y;
			indices[i++] = (x + 1) * (size + 1) + y;
			indices[i++] = (x + 1) * (size + 1) + y;
			indices[i++] = (x + 1) * (size + 1) + y + 1;
			indices[i++] = (x + 1) * (size + 1) + y + 1;
			indices[i++] = x * (size + 1) + y + 1;
			indices[i++] = x * (size + 1) + y + 1;
			indices[i++] = x * (size + 1) + y;
			indices[i++] = x * (size + 1) + y;
			indices[i++] = (x + 1) * (size + 1) + y + 1;
		}
	}
}