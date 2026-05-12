#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "common/arg_parser.h"
#include "common/file_utils.h"

typedef struct {
    int e;      // pattern from command line
    int i;      // ignore case
    int v;      // invert match
    int c;      // count matches
    int l;      // show only filenames with matches
    int n;      // show line numbers
    int h;      // suppress filename prefix
    int s;      // suppress errors
    int f;      // read patterns from file
    int o;      // show only matching part
} grep_flags;

void print_matching_part(const char *line, regex_t *regex, int ignore_case) {
    regmatch_t matches[1];
    const char *p = line;
    int offset = 0;
    
    while (regexec(regex, p, 1, matches, 0) == 0) {
        int start = matches[0].rm_so;
        int end = matches[0].rm_eo;
        
        // Print the matching part
        for (int i = start; i < end; i++) {
            putchar(p[i]);
        }
        putchar('\n');
        
        // Move past this match
        p += (end > 0) ? end : 1;
        offset += end;
    }
}

int match_line(const char *line, regex_t *regex) {
    return regexec(regex, line, 0, NULL, 0) == 0;
}

int process_file_grep(const char *filename, regex_t **patterns, int pattern_count, 
                      grep_flags flags, int file_index, int total_files) {
    FILE *file = file_open_safe(filename);
    if (!file) {
        if (!flags.s) {
            fprintf(stderr, "s21_grep: %s: No such file or directory\n", filename);
        }
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_num = 1;
    int match_count = 0;
    int printed_filename = 0;
    int has_match = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        // Remove trailing newline for processing
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        int matched = 0;
        for (int i = 0; i < pattern_count; i++) {
            if (match_line(line, patterns[i])) {
                matched = 1;
                break;
            }
        }

        if (flags.v) matched = !matched;

        if (matched) {
            has_match = 1;
            match_count++;
            
            if (flags.c) continue;  // Only count, don't print
            
            if (flags.l) {
                if (!printed_filename) {
                    printf("%s\n", filename);
                    printed_filename = 1;
                }
                break;  // Stop processing this file
            }
            
            // Print filename if multiple files and not -h
            if (total_files > 1 && !flags.h) {
                printf("%s:", filename);
            }
            
            // Print line number if -n
            if (flags.n) {
                printf("%d:", line_num);
            }
            
            // Print line or matching part
            if (flags.o && pattern_count > 0) {
                print_matching_part(line, patterns[0], flags.i);
            } else {
                printf("%s\n", line);
            }
        }
        
        line_num++;
    }

    if (flags.c && has_match) {
        if (total_files > 1 && !flags.h) {
            printf("%s:", filename);
        }
        printf("%d\n", match_count);
    } else if (flags.l && has_match) {
        // Already printed
    } else if (flags.c && !has_match && total_files == 1 && !flags.h) {
        printf("0\n");
    } else if (flags.c && !has_match && total_files > 1 && !flags.h) {
        printf("%s:0\n", filename);
    }

    free(line);
    file_close_safe(file);
    return 0;
}

int main(int argc, char *argv[]) {
    grep_flags flags = {0};
    char **patterns = NULL;
    int pattern_count = 0;
    char **files = NULL;
    int file_count = 0;
    char *pattern_file = NULL;

    if (parse_grep_args(argc, argv, &flags.e, &flags.i, &flags.v, &flags.c,
                        &flags.l, &flags.n, &flags.h, &flags.s, &flags.f,
                        &flags.o, &patterns, &pattern_count, &files, &file_count,
                        &pattern_file) != 0) {
        fprintf(stderr, "Usage: s21_grep [OPTION]... PATTERN [FILE]...\n");
        return 1;
    }

    // Compile regex patterns
    regex_t **compiled_patterns = malloc(sizeof(regex_t*) * pattern_count);
    int cflags = REG_EXTENDED;
    if (flags.i) cflags |= REG_ICASE;
    
    for (int i = 0; i < pattern_count; i++) {
        compiled_patterns[i] = malloc(sizeof(regex_t));
        if (regcomp(compiled_patterns[i], patterns[i], cflags) != 0) {
            fprintf(stderr, "s21_grep: Invalid regular expression: %s\n", patterns[i]);
            for (int j = 0; j <= i; j++) {
                regfree(compiled_patterns[j]);
                free(compiled_patterns[j]);
            }
            free(compiled_patterns);
            return 1;
        }
    }

    if (file_count == 0) {
        files = malloc(sizeof(char*));
        files[0] = strdup("-");
        file_count = 1;
    }

    int ret = 0;
    for (int i = 0; i < file_count; i++) {
        if (process_file_grep(files[i], compiled_patterns, pattern_count, 
                              flags, i, file_count) != 0) {
            ret = 1;
        }
        free(files[i]);
    }
    
    free(files);
    for (int i = 0; i < pattern_count; i++) {
        regfree(compiled_patterns[i]);
        free(compiled_patterns[i]);
        free(patterns[i]);
    }
    free(compiled_patterns);
    free(patterns);
    free(pattern_file);

    return ret;
}