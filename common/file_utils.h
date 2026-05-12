#ifndef COMMON_FILE_UTILS_H
#define COMMON_FILE_UTILS_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Open file, return stdin for "-" filename
// Returns NULL on error
FILE* file_open_safe(const char *filename);

// Close file if not stdin
void file_close_safe(FILE *file);

// Check if file is stdin
int file_is_stdin(FILE *file);

#ifdef __cplusplus
}
#endif

#endif  // COMMON_FILE_UTILS_H