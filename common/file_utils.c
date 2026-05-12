#define _POSIX_C_SOURCE 200809L
#include "file_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *file_open_safe(const char *path, const char *mode, int silent) {
    if (strcmp(path, "-") == 0) {
        return stdin;
    }
    FILE *fp = fopen(path, mode);
    if (!fp && !silent) {
        fprintf(stderr, "s21_utils: cannot open '%s'\n", path);
    }
    return fp;
}

ssize_t file_read_line(FILE *fp, char **line, size_t *len) {
    if (!fp) return -1;
    return getline(line, len, fp);
}

void file_close_safe(FILE *fp, const char *path) {
    (void)path;
    if (fp && fp != stdin) {
        fclose(fp);
    }
}

int file_is_stdin(FILE *fp) {
    return fp == stdin;
}