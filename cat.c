#define _POSIX_C_SOURCE 200809L
#include "common/arg_parser.h"
#include "common/file_utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void process_file(const char *path, const cat_flags *flags, int *line_num,
                         int *empty_line_count);
static void output_char(char ch, const cat_flags *flags, int *line_num,
                        int line_start, int *empty_line_count);

int main(int argc, char **argv) {
  cat_flags flags;
  char **files = NULL;
  int file_count = 0;

  if (parse_cat_args(argc, argv, &flags, &files, &file_count) != 0) {
    fprintf(stderr, "Usage: s21_cat [OPTION]... [FILE]...\n");
    return 1;
  }

  int line_num = 1;
  int empty_line_count = 0;

  for (int i = 0; i < file_count; ++i) {
    process_file(files[i], &flags, &line_num, &empty_line_count);
  }

  free_parsed_args(files, file_count);
  return 0;
}

static void process_file(const char *path, const cat_flags *flags, int *line_num,
                         int *empty_line_count) {
  FILE *fp = file_open_safe(path, "r", 0);
  if (!fp) return;

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  while ((read = file_read_line(fp, &line, &len)) != -1) {
    // Remove trailing newline for processing
    if (read > 0 && line[read - 1] == '\n') {
      line[read - 1] = '\0';
      read--;
    }

    int is_empty = (read == 0);
    int line_start = 1;

    // Handle squeeze blank lines
    if (flags->s && is_empty) {
      if (*empty_line_count >= 1) {
        continue;
      }
      (*empty_line_count)++;
    } else {
      *empty_line_count = 0;
    }

    // Output number if needed
    if (flags->b && !is_empty) {
      printf("%6d\t", (*line_num)++);
      line_start = 0;
    } else if (flags->n && !(flags->b && is_empty)) {
      printf("%6d\t", (*line_num)++);
      line_start = 0;
    }

    // Process each character
    for (ssize_t i = 0; i < read; ++i) {
      char ch = line[i];
      output_char(ch, flags, line_num, line_start, empty_line_count);
    }

    // Handle end of line
    if (flags->E) {
      printf("$");
    } else if (flags->e) {
      printf("$");
    }
    printf("\n");
  }

  free(line);
  file_close_safe(fp, path);
}

static void output_char(char ch, const cat_flags *flags, int *line_num,
                        int line_start, int *empty_line_count) {
  (void)line_num;        // unused
  (void)empty_line_count; // unused

  if (flags->v) {
    // Non-printing characters
    if (ch == '\t') {
      if (flags->T || flags->t) {
        printf("^I");
      } else {
        putchar(ch);
      }
    } else if (ch >= 0 && ch < 32 && ch != '\n') {
      printf("^%c", ch + 64);  // Control character
    } else if (ch == 127) {
      printf("^?");
    } else {
      putchar(ch);
    }
  } else if (flags->T && ch == '\t') {
    printf("^I");
  } else {
    putchar(ch);
  }
}