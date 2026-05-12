#define _POSIX_C_SOURCE 200809L
#include "arg_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

static int add_to_array(char ***arr, int *count, const char *str) {
  char **new_arr = realloc(*arr, (*count + 1) * sizeof(char *));
  if (!new_arr) return -1;
  *arr = new_arr;
  (*arr)[*count] = strdup(str);
  if (!(*arr)[*count]) return -1;
  (*count)++;
  return 0;
}

int parse_cat_args(int argc, char **argv, cat_flags *flags, char ***files, int *file_count) {
  if (!flags || !files || !file_count) return -1;
  memset(flags, 0, sizeof(cat_flags));
  *files = NULL;
  *file_count = 0;

  static struct option long_opts[] = {
    {"number-nonblank", no_argument, 0, 'b'},
    {"number", no_argument, 0, 'n'},
    {"squeeze-blank", no_argument, 0, 's'},
    {"show-ends", no_argument, 0, 'E'},
    {"show-tabs", no_argument, 0, 'T'},
    {"show-nonprinting", no_argument, 0, 'v'},
    {0, 0, 0, 0}
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "beEnstTv", long_opts, NULL)) != -1) {
    switch (opt) {
      case 'b': flags->b = 1; break;
      case 'e': flags->e = 1; flags->v = 1; break;
      case 'E': flags->E = 1; break;
      case 'n': flags->n = 1; break;
      case 's': flags->s = 1; break;
      case 't': flags->t = 1; flags->v = 1; break;
      case 'T': flags->T = 1; break;
      case 'v': flags->v = 1; break;
      default: return -1;
    }
  }

  for (int i = optind; i < argc; ++i) {
    if (add_to_array(files, file_count, argv[i]) != 0) {
      free_parsed_args(*files, *file_count);
      *files = NULL;
      *file_count = 0;
      return -1;
    }
  }

  if (*file_count == 0) {
    if (add_to_array(files, file_count, "-") != 0) return -1;
  }

  return 0;
}

int parse_grep_args(int argc, char **argv, grep_flags *flags, char ***patterns, 
                    int *pattern_count, char ***files, int *file_count) {
  if (!flags || !patterns || !pattern_count || !files || !file_count) return -1;
  memset(flags, 0, sizeof(grep_flags));
  *patterns = NULL;
  *pattern_count = 0;
  *files = NULL;
  *file_count = 0;

  static struct option long_opts[] = {
    {"ignore-case", no_argument, 0, 'i'},
    {"invert-match", no_argument, 0, 'v'},
    {"count", no_argument, 0, 'c'},
    {"files-with-matches", no_argument, 0, 'l'},
    {"line-number", no_argument, 0, 'n'},
    {"no-filename", no_argument, 0, 'h'},
    {"no-messages", no_argument, 0, 's'},
    {"regexp", required_argument, 0, 'e'},
    {"file", required_argument, 0, 'f'},
    {"only-matching", no_argument, 0, 'o'},
    {0, 0, 0, 0}
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "e:ivclnhsf:o", long_opts, NULL)) != -1) {
    switch (opt) {
      case 'e': 
        flags->e = 1; 
        if (add_to_array(patterns, pattern_count, optarg) != 0) goto error; 
        break;
      case 'i': flags->i = 1; break;
      case 'v': flags->v = 1; break;
      case 'c': flags->c = 1; break;
      case 'l': flags->l = 1; break;
      case 'n': flags->n = 1; break;
      case 'h': flags->h = 1; break;
      case 's': flags->s = 1; break;
      case 'f': 
        flags->f = 1;
        FILE *fp = fopen(optarg, "r");
        if (!fp) {
          if (!flags->s) fprintf(stderr, "grep: %s: No such file\n", optarg);
          goto error;
        }
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        while ((read = getline(&line, &len, fp)) != -1) {
          if (read > 0 && line[read-1] == '\n') line[read-1] = '\0';
          if (add_to_array(patterns, pattern_count, line) != 0) {
            free(line);
            fclose(fp);
            goto error;
          }
        }
        free(line);
        fclose(fp);
        break;
      case 'o': flags->o = 1; break;
      default: goto error;
    }
  }

  if (*pattern_count == 0 && optind < argc) {
    if (add_to_array(patterns, pattern_count, argv[optind]) != 0) goto error;
    optind++;
  }

  if (*pattern_count == 0) goto error;

  for (int i = optind; i < argc; ++i) {
    if (add_to_array(files, file_count, argv[i]) != 0) goto error;
  }

  if (*file_count == 0) {
    if (add_to_array(files, file_count, "-") != 0) goto error;
  }

  return 0;

error:
  free_parsed_args(*patterns, *pattern_count);
  free_parsed_args(*files, *file_count);
  *patterns = NULL;
  *files = NULL;
  *pattern_count = 0;
  *file_count = 0;
  return -1;
}

void free_parsed_args(char **arr, int count) {
  if (!arr) return;
  for (int i = 0; i < count; ++i) free(arr[i]);
  free(arr);
}