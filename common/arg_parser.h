#ifndef COMMON_ARG_PARSER_H
#define COMMON_ARG_PARSER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Flags for cat utility
typedef struct {
  bool b;  // number non-empty output lines
  bool e;  // display $ at end of lines, implies -v
  bool E;  // display $ at end of lines (without -v)
  bool n;  // number all output lines
  bool s;  // squeeze multiple empty lines
  bool t;  // display TAB as ^I, implies -v
  bool T;  // display TAB as ^I (without -v)
  bool v;  // non-printing characters ^ and M-
} cat_flags;

// Flags for grep utility
typedef struct {
  bool e;        // use PATTERNS as patterns (multiple allowed)
  bool i;        // ignore case
  bool v;        // invert match
  bool c;        // count lines
  bool l;        // print file names only
  bool n;        // print line numbers
  bool h;        // suppress file name prefix
  bool s;        // suppress errors
  bool f;        // read patterns from FILE
  bool o;        // show only matching part
} grep_flags;

// Parse arguments for cat
int parse_cat_args(int argc, char **argv, cat_flags *flags, char ***files, int *file_count);

// Parse arguments for grep
int parse_grep_args(int argc, char **argv, grep_flags *flags, char ***patterns, 
                    int *pattern_count, char ***files, int *file_count);

// Free allocated argument arrays
void free_parsed_args(char **arr, int count);

#ifdef __cplusplus
}
#endif

#endif  // COMMON_ARG_PARSER_H