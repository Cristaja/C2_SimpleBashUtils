#ifndef COMMON_ARG_PARSER_H
#define COMMON_ARG_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

// Parse cat command line arguments
// Returns 0 on success, non-zero on error
int parse_cat_args(int argc, char *argv[], int *b, int *e, int *E, int *n, 
                   int *s, int *t, int *T, int *v, char ***files, int *file_count);

// Parse grep command line arguments
// Returns 0 on success, non-zero on error
int parse_grep_args(int argc, char *argv[], int *e, int *i, int *v, int *c,
                    int *l, int *n, int *h, int *s, int *f, int *o,
                    char ***patterns, int *pattern_count, char ***files, 
                    int *file_count, char **pattern_file);

// Free parsed arguments arrays
void free_parsed_args(char **files, int file_count, char **patterns, int pattern_count);

#ifdef __cplusplus
}
#endif

#endif  // COMMON_ARG_PARSER_H