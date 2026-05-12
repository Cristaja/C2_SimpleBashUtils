#define _GNU_SOURCE
#include "arg_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_cat_args(int argc, char *argv[], int *b, int *e, int *E, int *n,
                   int *s, int *t, int *T, int *v, char ***files, int *file_count) {
    *files = NULL;
    *file_count = 0;
    
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // Handle combined flags like -ben
            for (int j = 1; argv[i][j]; j++) {
                switch (argv[i][j]) {
                    case 'b': *b = 1; break;
                    case 'e': *e = 1; *v = 1; break;
                    case 'E': *E = 1; break;
                    case 'n': *n = 1; break;
                    case 's': *s = 1; break;
                    case 't': *t = 1; *v = 1; break;
                    case 'T': *T = 1; break;
                    case 'v': *v = 1; break;
                    default:
                        fprintf(stderr, "s21_cat: invalid option -- '%c'\n", argv[i][j]);
                        return 1;
                }
            }
        } else {
            // File argument
            (*file_count)++;
            *files = realloc(*files, sizeof(char*) * (*file_count));
            (*files)[*file_count - 1] = strdup(argv[i]);
        }
    }
    
    return 0;
}

int parse_grep_args(int argc, char *argv[], int *e, int *i, int *v, int *c,
                    int *l, int *n, int *h, int *s, int *f, int *o,
                    char ***patterns, int *pattern_count, char ***files, 
                    int *file_count, char **pattern_file) {
    *patterns = NULL;
    *pattern_count = 0;
    *files = NULL;
    *file_count = 0;
    *pattern_file = NULL;
    
    int pattern_provided = 0;
    char *single_pattern = NULL;
    
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] != '\0') {
            // Handle combined flags
            for (int j = 1; argv[i][j]; j++) {
                switch (argv[i][j]) {
                    case 'e': 
                        *e = 1;
                        if (i + 1 >= argc) {
                            fprintf(stderr, "s21_grep: option requires an argument -- 'e'\n");
                            return 1;
                        }
                        (*pattern_count)++;
                        *patterns = realloc(*patterns, sizeof(char*) * (*pattern_count));
                        (*patterns)[*pattern_count - 1] = strdup(argv[++i]);
                        pattern_provided = 1;
                        break;
                    case 'i': *i = 1; break;
                    case 'v': *v = 1; break;
                    case 'c': *c = 1; break;
                    case 'l': *l = 1; break;
                    case 'n': *n = 1; break;
                    case 'h': *h = 1; break;
                    case 's': *s = 1; break;
                    case 'f':
                        *f = 1;
                        if (i + 1 >= argc) {
                            fprintf(stderr, "s21_grep: option requires an argument -- 'f'\n");
                            return 1;
                        }
                        free(*pattern_file);
                        *pattern_file = strdup(argv[++i]);
                        pattern_provided = 1;
                        break;
                    case 'o': *o = 1; break;
                    default:
                        fprintf(stderr, "s21_grep: invalid option -- '%c'\n", argv[i][j]);
                        return 1;
                }
            }
        } else {
            // Check if this is the pattern (when no -e or -f)
            if (!pattern_provided && *pattern_count == 0) {
                single_pattern = strdup(argv[i]);
                pattern_provided = 1;
            } else {
                // File argument
                (*file_count)++;
                *files = realloc(*files, sizeof(char*) * (*file_count));
                (*files)[*file_count - 1] = strdup(argv[i]);
            }
        }
    }
    
    // Handle single pattern (not from -e or -f)
    if (single_pattern) {
        (*pattern_count)++;
        *patterns = realloc(*patterns, sizeof(char*) * (*pattern_count));
        (*patterns)[*pattern_count - 1] = single_pattern;
    }
    
    // Read patterns from file if -f specified
    if (*f && *pattern_file) {
        FILE *pf = fopen(*pattern_file, "r");
        if (!pf) {
            fprintf(stderr, "s21_grep: %s: No such file or directory\n", *pattern_file);
            return 1;
        }
        
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        while ((read = getline(&line, &len, pf)) != -1) {
            if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
            if (strlen(line) > 0) {
                (*pattern_count)++;
                *patterns = realloc(*patterns, sizeof(char*) * (*pattern_count));
                (*patterns)[*pattern_count - 1] = strdup(line);
            }
        }
        free(line);
        fclose(pf);
    }
    
    if (*pattern_count == 0) {
        fprintf(stderr, "s21_grep: No pattern provided\n");
        return 1;
    }
    
    return 0;
}

void free_parsed_args(char **files, int file_count, char **patterns, int pattern_count) {
    for (int i = 0; i < file_count; i++) free(files[i]);
    free(files);
    for (int i = 0; i < pattern_count; i++) free(patterns[i]);
    free(patterns);
}