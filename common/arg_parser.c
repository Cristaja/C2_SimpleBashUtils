#define _POSIX_C_SOURCE 200809L
#include "arg_parser.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------------------------
// cat argument parsing
// ----------------------------------------------------------------------
int parse_cat_args(int argc, char **argv, cat_flags *flags,
                   char ***files, int *file_count) {
    // Initialize flags to false
    memset(flags, 0, sizeof(cat_flags));

    // Long options mapping (GNU style)
    static struct option long_opts[] = {
        {"number-nonblank", no_argument, NULL, 'b'},
        {"number",          no_argument, NULL, 'n'},
        {"squeeze-blank",   no_argument, NULL, 's'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "beEnstTv", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'b': flags->b = true; break;
            case 'e': flags->e = true; flags->v = true; break;
            case 'E': flags->E = true; break;
            case 'n': flags->n = true; break;
            case 's': flags->s = true; break;
            case 't': flags->t = true; flags->v = true; break;
            case 'T': flags->T = true; break;
            case 'v': flags->v = true; break;
            default: return -1;
        }
    }

    // Remaining arguments are file names
    *file_count = argc - optind;
    if (*file_count == 0) {
        // No files -> read from stdin
        *files = malloc(sizeof(char*));
        (*files)[0] = strdup("-");
        *file_count = 1;
    } else {
        *files = malloc((*file_count) * sizeof(char*));
        for (int i = 0; i < *file_count; ++i) {
            (*files)[i] = strdup(argv[optind + i]);
        }
    }
    return 0;
}

// ----------------------------------------------------------------------
// grep argument parsing
// ----------------------------------------------------------------------
int parse_grep_args(int argc, char **argv, grep_flags *flags,
                    char ***patterns, int *pattern_count,
                    char ***files, int *file_count) {
    memset(flags, 0, sizeof(grep_flags));

    // Temporary storage for patterns
    char **pat_list = NULL;
    int pat_cap = 0;
    *pattern_count = 0;

    static struct option long_opts[] = {
        {"ignore-case", no_argument, NULL, 'i'},
        {"invert-match", no_argument, NULL, 'v'},
        {"count", no_argument, NULL, 'c'},
        {"files-with-matches", no_argument, NULL, 'l'},
        {"line-number", no_argument, NULL, 'n'},
        {"no-filename", no_argument, NULL, 'h'},
        {"no-messages", no_argument, NULL, 's'},
        {"regexp", required_argument, NULL, 'e'},
        {"file", required_argument, NULL, 'f'},
        {"only-matching", no_argument, NULL, 'o'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "e:ivclnhsf:o", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'e':
                flags->e = true;
                if (*pattern_count >= pat_cap) {
                    pat_cap = pat_cap ? pat_cap * 2 : 4;
                    pat_list = realloc(pat_list, pat_cap * sizeof(char*));
                }
                pat_list[(*pattern_count)++] = strdup(optarg);
                break;
            case 'i': flags->i = true; break;
            case 'v': flags->v = true; break;
            case 'c': flags->c = true; break;
            case 'l': flags->l = true; break;
            case 'n': flags->n = true; break;
            case 'h': flags->h = true; break;
            case 's': flags->s = true; break;
            case 'f':
                flags->f = true;
                // Read patterns from file
                {
                    FILE *fp = fopen(optarg, "r");
                    if (!fp) {
                        if (!flags->s) fprintf(stderr, "s21_grep: %s: No such file\n", optarg);
                        return -1;
                    }
                    char *line = NULL;
                    size_t len = 0;
                    ssize_t read;
                    while ((read = getline(&line, &len, fp)) != -1) {
                        if (read > 0 && line[read-1] == '\n') line[read-1] = '\0';
                        if (*pattern_count >= pat_cap) {
                            pat_cap = pat_cap ? pat_cap * 2 : 4;
                            pat_list = realloc(pat_list, pat_cap * sizeof(char*));
                        }
                        pat_list[(*pattern_count)++] = strdup(line);
                    }
                    free(line);
                    fclose(fp);
                }
                break;
            case 'o': flags->o = true; break;
            default: return -1;
        }
    }

    // If no -e and no -f, the first non-option argument is the pattern
    if (*pattern_count == 0 && optind < argc) {
        pat_list = malloc(sizeof(char*));
        pat_list[0] = strdup(argv[optind]);
        *pattern_count = 1;
        optind++;
    }

    if (*pattern_count == 0) {
        fprintf(stderr, "s21_grep: No pattern provided\n");
        return -1;
    }

    *patterns = pat_list;

    // Remaining arguments are files
    *file_count = argc - optind;
    if (*file_count == 0) {
        *files = malloc(sizeof(char*));
        (*files)[0] = strdup("-");
        *file_count = 1;
    } else {
        *files = malloc((*file_count) * sizeof(char*));
        for (int i = 0; i < *file_count; ++i) {
            (*files)[i] = strdup(argv[optind + i]);
        }
    }

    return 0;
}

void free_parsed_args(char **arr, int count) {
    if (!arr) return;
    for (int i = 0; i < count; ++i) free(arr[i]);
    free(arr);
}