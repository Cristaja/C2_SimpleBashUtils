#include "file_utils.h"
#include <string.h>

FILE* file_open_safe(const char *filename) {
    if (strcmp(filename, "-") == 0) {
        return stdin;
    }
    return fopen(filename, "r");
}

void file_close_safe(FILE *file) {
    if (file && file != stdin) {
        fclose(file);
    }
}

int file_is_stdin(FILE *file) {
    return file == stdin;
}