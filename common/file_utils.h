#ifndef COMMON_FILE_UTILS_H
#define COMMON_FILE_UTILS_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

FILE *file_open_safe(const char *path, const char *mode, int quiet);
ssize_t file_read_line(FILE *fp, char **lineptr, size_t *n);
void file_close_safe(FILE *fp, const char *path);
int file_is_stdin(FILE *fp);

#ifdef __cplusplus
}
#endif

#endif  // COMMON_FILE_UTILS_H