#ifndef COMMON_FILE_UTILS_H
#define COMMON_FILE_UTILS_H

#include <stdio.h>

// Open file for reading; if path is "-", returns stdin.
// If silent is true, suppresses error messages.
FILE *file_open_safe(const char *path, const char *mode, int silent);

// Read a line from fp using getline. Returns number of characters read (including newline),
// or -1 on EOF/error.
ssize_t file_read_line(FILE *fp, char **line, size_t *len);

// Close file (does nothing if fp == stdin).
void file_close_safe(FILE *fp, const char *path);

// Check if FILE* is standard input (used to avoid fclose on stdin).
int file_is_stdin(FILE *fp);

#endif // COMMON_FILE_UTILS_H