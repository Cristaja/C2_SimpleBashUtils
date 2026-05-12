#ifndef COMMON_ARG_PARSER_H
#define COMMON_ARG_PARSER_H

#include <stdbool.h>

// CAT flags structure
typedef struct {
    bool b; // number non-empty lines
    bool e; // show $ and imply -v
    bool E; // show $ only
    bool n; // number all lines
    bool s; // squeeze blank lines
    bool t; // show tabs as ^I and imply -v
    bool T; // show tabs as ^I only
    bool v; // show non-printing
} cat_flags;

// GREP flags structure
typedef struct {
    bool e; // pattern provided via -e
    bool i; // case-insensitive
    bool v; // invert match
    bool c; // count matches
    bool l; // list filenames with at least one match
    bool n; // show line numbers
    bool h; // suppress filename prefix
    bool s; // suppress errors
    bool f; // read patterns from file
    bool o; // print only matching parts
} grep_flags;

// Parse arguments for s21_cat
int parse_cat_args(int argc, char **argv, cat_flags *flags,
                   char ***files, int *file_count);

// Parse arguments for s21_grep
int parse_grep_args(int argc, char **argv, grep_flags *flags,
                    char ***patterns, int *pattern_count,
                    char ***files, int *file_count);

// Free dynamically allocated arrays (patterns/files)
void free_parsed_args(char **arr, int count);

#endif // COMMON_ARG_PARSER_H