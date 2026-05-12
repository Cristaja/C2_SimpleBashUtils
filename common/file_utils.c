#define _POSIX_C_SOURCE 200809L
#include "file_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *file_open_safe(const char *path, const char *mode, int quiet) {
  if (!path) return NULL;
  if (strcmp(path, "-") == 0) {
    return stdin;
  }
  FILE *fp = fopen(path, mode);
  if (!fp && !quiet) {
    fprintf(stderr, "Cannot open file: %s\n", path);
  }
  return fp;
}

ssize_t file_read_line(FILE *fp, char **lineptr, size_t *n) {
  if (!fp || !lineptr || !n) return -1;
  return getline(lineptr, n, fp);
}

void file_close_safe(FILE *fp, const char *path) {
  if (!fp) return;
  if (strcmp(path, "-") != 0) {
    fclose(fp);
  }
}

int file_is_stdin(FILE *fp) {
  return fp == stdin;
}