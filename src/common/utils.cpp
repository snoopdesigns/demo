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