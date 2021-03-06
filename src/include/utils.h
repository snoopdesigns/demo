#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "../include/log.h"

extern char *read_from_file(const char *filename);
extern void generateTexture(GLbyte* graph, int n);
extern void generateVerticesMesh(glm::vec2* vertices, int size, int scale, int strategy);
extern int splitVerticesMesh(glm::vec2* vertices, int size, int maxSegmentSize, glm::vec2* result,int* segmentSizes);
extern void generateTrianglesIndices(GLushort* indices, int sizex, int sizey);
extern void generateLinesIndices(GLushort* indices, int sizex, int sizey);

#endif
