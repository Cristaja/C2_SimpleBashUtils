#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "common/arg_parser.h"
#include "common/file_utils.h"

typedef struct {
    int b;  // number non-empty lines
    int e;  // show $ at end of lines
    int E;  // same as e (GNU extension)
    int n;  // number all lines
    int s;  // squeeze empty lines
    int t;  // show tabs as ^I
    int T;  // same as t (GNU extension)
    int v;  // show non-printing
} cat_flags;

void print_char(FILE *out, char c, int show_non_printing) {
    if (!show_non_printing) {
        fputc(c, out);
        return;
    }

    unsigned char ch = (unsigned char)c;
    if (ch >= 128) {  // non-ASCII
        fprintf(out, "M-");
        ch -= 128;
    }

    if (ch < 32) {  // control characters
        if (ch == 9) {
            fputc('\t', out);
        } else if (ch == 10) {
            fputc('\n', out);
        } else {
            fprintf(out, "^%c", ch + 64);
        }
    } else if (ch == 127) {
        fprintf(out, "^?");
    } else {
        fputc(c, out);
    }
}

int process_file(const char *filename, cat_flags flags) {
    FILE *file = file_open_safe(filename);
    if (!file) return 1;

    char *line = NULL;
    size_t len = 0;
    ssize_t bytes_read;
    int line_num = 1;
    int prev_empty = 0;

    while ((bytes_read = getline(&line, &len, file)) != -1) {
        // Remove trailing newline for processing
        int has_newline = (bytes_read > 0 && line[bytes_read - 1] == '\n');
        if (has_newline) line[bytes_read - 1] = '\0';

        int is_empty = (bytes_read == 1 && has_newline) || 
                       (has_newline && bytes_read == 1) ||
                       (bytes_read == 0 && has_newline) ||
                       (strlen(line) == 0);

        // Squeeze empty lines
        if (flags.s && is_empty) {
            if (prev_empty) {
                if (has_newline) fputc('\n', stdout);
                prev_empty = 1;
                continue;
            }
            prev_empty = 1;
        } else {
            prev_empty = 0;
        }

        // Print line number
        int should_print_number = 0;
        if (flags.n && !(flags.b && is_empty)) {
            should_print_number = 1;
        } else if (flags.b && !is_empty) {
            should_print_number = 1;
        }

        if (should_print_number) {
            printf("%6d\t", line_num++);
        } else if (flags.n) {
            printf("%6d\t", line_num++);
        }

        // Print line content with special characters
        for (char *p = line; *p; p++) {
            if ((flags.t || flags.T) && *p == '\t') {
                printf("^I");
            } else if ((flags.e || flags.E) && *p == '$') {
                fputc('$', stdout);
            } else if (flags.v) {
                print_char(stdout, *p, 1);
            } else {
                fputc(*p, stdout);
            }
        }

        // Add $ at line end if e or E flag
        if ((flags.e || flags.E) && has_newline) {
            fputc('$', stdout);
        }

        if (has_newline) fputc('\n', stdout);
    }

    free(line);
    file_close_safe(file);
    return 0;
}

int main(int argc, char *argv[]) {
    cat_flags flags = {0};
    char **files = NULL;
    int file_count = 0;

    if (parse_cat_args(argc, argv, &flags.b, &flags.e, &flags.E, 
                       &flags.n, &flags.s, &flags.t, &flags.T, &flags.v,
                       &files, &file_count) != 0) {
        fprintf(stderr, "Usage: s21_cat [OPTION]... [FILE]...\n");
        fprintf(stderr, "Options: -b, -e, -E, -n, -s, -t, -T, -v\n");
        return 1;
    }

    // Handle GNU aliases
    if (flags.E) flags.e = 1;
    if (flags.T) flags.t = 1;

    if (file_count == 0) {
        // Read from stdin
        files = malloc(sizeof(char*));
        files[0] = strdup("-");
        file_count = 1;
    }

    int ret = 0;
    for (int i = 0; i < file_count; i++) {
        if (process_file(files[i], flags) != 0) {
            ret = 1;
        }
        free(files[i]);
    }
    free(files);

    return ret;
}