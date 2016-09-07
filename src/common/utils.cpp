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

void printVerticesArray(glm::vec2* array, int sizex, int sizey) {
	gl_log("sizex = %d, sizey = %d\n", sizex, sizey);
	for(int i=0;i<sizex;i++) {
		for(int j=0;j<sizey;j++) {
		    gl_log("Vertex [%d, %d] : [%f, %f]\n", i, j, array[i*sizey+j].x, array[i*sizey+j].y);
		}
	}
}

void printLineIndicesArray(GLushort* array, int sizex, int sizey) {
	gl_log("LINEINDICE sizex = %d, sizey = %d\n", sizex, sizey);
	for(int i=0;i<sizex;i++) {
		for(int j=0;j<sizey;j++) {
		    gl_log("Line [%d, %d] : [%d]\n", i, j, array[i*sizey+j]);
		}
		gl_log("Next row\n");
	}
}

void generateTexture(GLbyte* graph, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			graph[i*size+j] = roundf((float)rand()/(float)(RAND_MAX) * 127 + 128);
			if(DEBUG) gl_log("Generated texture value: %f\n", roundf((float)rand()/(float)(RAND_MAX) * 127 + 128));
		}
	}
}

void generateVerticesMesh(glm::vec2* vertices, int size, int scale) {
	float ratio_x = (2.0 * scale) / (size-1);
	float ratio_y = (2.0 * scale) / (size-1);
	
	
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			vertices[i*size+j].x = (-1.0*scale) + j * ratio_x;
			vertices[i*size+j].y = (1.0*scale) - i * ratio_y;
			if(DEBUG) gl_log("Vertex generated: [%f, %f]\n", vertices[i*size+j].x, vertices[i*size+j].y);
		}
	}
}

int splitVerticesMesh(glm::vec2* vertices, int size, int maxSegmentSize, glm::vec2* result,int* segmentSizes) {
	long num = 0;
	int currentSegment = 0;
    for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result[currentSegment * maxSegmentSize + num].x = vertices[i*size+j].x;
			result[currentSegment * maxSegmentSize + num].y = vertices[i*size+j].y;
			if(DEBUG) gl_log("Adding elem [%d, %d] to index: [%d, %d]. Elem value: [%f, %f]\n", i, j, currentSegment, num,vertices[i*size+j].x,vertices[i*size+j].y);
		    num++;
			if(j==size-1) {
			    if(num + size > maxSegmentSize) {
				    if(DEBUG) gl_log("Number of vertices in segment %d: %d. Size of mesh: %d. Rows in segment: %d\n", currentSegment, num, size, num/size);
					segmentSizes[currentSegment] = num/size;
					if(num/size<=1) {printf("Wrong config: size of segment must be > 1!\n"); exit(0);}
				    currentSegment ++;
					num = 0;
					i--;
					if(DEBUG) gl_log("Starting new segment\n");
				}
			}
		}
	}
	segmentSizes[currentSegment] = num/size;
	if(num/size<=1) {printf("Wrong config: size of segment must be > 1!\n"); exit(0);}
	if(segmentSizes[currentSegment]==1) currentSegment--;
	if(DEBUG) gl_log("Number of vertices in segment %d: %d. Size of mesh: %d. Rows in segment: %d\n", currentSegment, num, size, num/size);
	if(DEBUG) gl_log("Total segments: %d\n", currentSegment+1);
	return currentSegment+1;
}

void generateTrianglesIndices(GLushort* indices, int sizex, int sizey) {
    unsigned long i = 0;
	for (int y = 0; y < sizey; y++) {
		for (int x = 0; x < sizex; x++) {
			indices[i++] = y * (sizex + 1) + x;
			indices[i++] = (y + 1) * (sizex + 1) + x;
			indices[i++] = (y + 1) * (sizex + 1) + x + 1;
			indices[i++] = (y + 1) * (sizex + 1) + x + 1;
			indices[i++] = y * (sizex + 1) + x + 1;
			indices[i++] = y * (sizex + 1) + x;
			if(DEBUG) gl_log("Triangle index: [%d,%d,%d,%d,%d,%d]\n", indices[i-6],indices[i-5],indices[i-4],indices[i-3],indices[i-2],indices[i-1]);
		}
	}
}

void generateLinesIndices(GLushort* indices, int sizex, int sizey) {
    unsigned long i = 0;
	for (int y = 0; y < sizey; y++) {
		for (int x = 0; x < sizex; x++) {
			indices[i++] = y * (sizex+1) + x;
			indices[i++] = (y + 1) * (sizex+1) + x;
			indices[i++] = (y + 1) * (sizex+1) + x;
			indices[i++] = (y + 1) * (sizex+1) + x + 1;
			indices[i++] = (y + 1) * (sizex+1) + x + 1;
			indices[i++] = y * (sizex+1) + x + 1;
			indices[i++] = y * (sizex+1) + x + 1;
			indices[i++] = y * (sizex+1) + x;
			indices[i++] = y * (sizex+1) + x;
			indices[i++] = (y + 1) * (sizex+1) + x + 1;
			if(DEBUG) gl_log("Line index: [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\n", indices[i-10],indices[i-9],indices[i-8],indices[i-7],indices[i-6],indices[i-5],indices[i-4],indices[i-3],indices[i-2],indices[i-1]);
		}
	}
}
