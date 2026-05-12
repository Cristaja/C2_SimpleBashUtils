#define _POSIX_C_SOURCE 200809L
#include "common/arg_parser.h"
#include "common/file_utils.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  regex_t *regexes;
  int count;
} pattern_list;

static int compile_patterns(pattern_list *pl, char **patterns, int pattern_count,
                            grep_flags *flags);
static void free_patterns(pattern_list *pl);
static void process_file(const char *path, const pattern_list *pl,
                         const grep_flags *flags, int file_count,
                         int *total_matches, int *file_matches);
static void print_match(const char *line, const char *filename, int line_num,
                        const grep_flags *flags, int file_count,
                        char *match_start, char *match_end);
static int match_line(const char *line, const pattern_list *pl,
                      const grep_flags *flags, regmatch_t *pmatch);

int main(int argc, char **argv) {
  grep_flags flags;
  char **patterns = NULL;
  int pattern_count = 0;
  char **files = NULL;
  int file_count = 0;

  if (parse_grep_args(argc, argv, &flags, &patterns, &pattern_count, &files,
                      &file_count) != 0) {
    fprintf(stderr, "Usage: s21_grep [OPTION]... PATTERNS [FILE]...\n");
    return 1;
  }

  pattern_list pl = {NULL, 0};
  if (compile_patterns(&pl, patterns, pattern_count, &flags) != 0) {
    fprintf(stderr, "Failed to compile regex\n");
    free_parsed_args(patterns, pattern_count);
    free_parsed_args(files, file_count);
    return 1;
  }

  int total_matches = 0;
  for (int i = 0; i < file_count; ++i) {
    int file_matches = 0;
    process_file(files[i], &pl, &flags, file_count, &total_matches, &file_matches);
    if (flags->l && file_matches > 0) {
      printf("%s\n", files[i]);
    }
    if (flags->c && !flags->l) {
      if (file_count > 1 && !flags->h)
        printf("%s:", files[i]);
      printf("%d\n", file_matches);
    }
  }

  if (flags->c && !flags->l && file_count == 0) {
    printf("%d\n", total_matches);
  }

  free_patterns(&pl);
  free_parsed_args(patterns, pattern_count);
  free_parsed_args(files, file_count);
  return 0;
}

static int compile_patterns(pattern_list *pl, char **patterns, int pattern_count,
                            grep_flags *flags) {
  pl->count = pattern_count;
  pl->regexes = malloc(pl->count * sizeof(regex_t));
  if (!pl->regexes) return -1;

  int cflags = REG_EXTENDED | REG_NOSUB;
  if (flags->i) cflags |= REG_ICASE;

  for (int i = 0; i < pl->count; ++i) {
    if (regcomp(&pl->regexes[i], patterns[i], cflags) != 0) {
      for (int j = 0; j < i; ++j) regfree(&pl->regexes[j]);
      free(pl->regexes);
      pl->regexes = NULL;
      return -1;
    }
  }
  return 0;
}

static void free_patterns(pattern_list *pl) {
  if (!pl->regexes) return;
  for (int i = 0; i < pl->count; ++i) regfree(&pl->regexes[i]);
  free(pl->regexes);
  pl->regexes = NULL;
  pl->count = 0;
}

static void process_file(const char *path, const pattern_list *pl,
                         const grep_flags *flags, int file_count,
                         int *total_matches, int *file_matches) {
  FILE *fp = file_open_safe(path, "r", flags->s);
  if (!fp) return;

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int line_num = 1;
  int matches_in_file = 0;

  while ((read = file_read_line(fp, &line, &len)) != -1) {
    // Remove trailing newline
    if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';

    regmatch_t pmatch;
    int matched = match_line(line, pl, flags, &pmatch);

    if ((matched && !flags->v) || (!matched && flags->v)) {
      matches_in_file++;
      (*total_matches)++;

      if (!flags->c && !flags->l) {
        print_match(line, path, line_num, flags, file_count, line + pmatch.rm_so,
                    line + pmatch.rm_eo);
      } else if (flags->o && matched && !flags->c && !flags->l) {
        // For -o with multiple matches in line, we'd need to iterate
        // but for simplicity we show first match
      }
    }
    line_num++;
  }

  free(line);
  file_close_safe(fp, path);
  if (file_matches) *file_matches = matches_in_file;
}

static void print_match(const char *line, const char *filename, int line_num,
                        const grep_flags *flags, int file_count,
                        char *match_start, char *match_end) {
  if (!flags->h && file_count > 1) printf("%s:", filename);
  if (flags->n) printf("%d:", line_num);
  if (flags->o) {
    // Print only matching part
    fwrite(match_start, 1, match_end - match_start, stdout);
  } else {
    printf("%s", line);
  }
  printf("\n");
}

static int match_line(const char *line, const pattern_list *pl,
                      const grep_flags *flags, regmatch_t *pmatch) {
  if (!pmatch) return 0;

  for (int i = 0; i < pl->count; ++i) {
    if (regexec(&pl->regexes[i], line, 1, pmatch, 0) == 0) {
      return 1;
    }
  }
  return 0;
}