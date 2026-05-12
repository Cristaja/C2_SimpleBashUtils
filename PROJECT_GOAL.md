# Project Goal: Simple Bash Utilities (cat and grep)

Develop two standard Unix text processing utilities – `cat` and `grep` – in the C programming language (C11 standard).  
The goal is to replicate the core functionality of the original bash tools with the following constraints:

- Strict adherence to structural programming principles.
- Code must be portable, avoid deprecated language constructs, and follow POSIX.1-2017.
- Use only allowed libraries (POSIX regex or PCRE2 for `grep`).
- Provide a clean Makefile with targets `s21_cat` and `s21_grep`.
- Include integration tests that compare the behaviour of the implemented tools with the **busybox** versions (Alpine 3.20).
- Organise the code into reusable modules to avoid duplication, placing common code in `src/common/`.

The final deliverables are two executables:  
`src/cat/s21_cat` and `src/grep/s21_grep`.

---

REQUIREMENTS.md
# Requirements for `s21_cat` and `s21_grep`

## Functional Requirements

### `s21_cat`
- [ ] Support the following flags (including GNU long versions where applicable):
  - `-b` (--number-nonblank) – number only non‑empty output lines.
  - `-e` (implies `-v`) – display `$` at the end of each line; for Windows line endings (`\r\n`) show `^M$`.
  - `-E` (GNU only, does **not** imply `-v`) – same as `-e` but without `-v`.
  - `-n` (--number) – number all output lines.
  - `-s` (--squeeze-blank) – suppress repeated empty output lines.
  - `-t` (implies `-v`) – display tab characters as `^I`.
  - `-T` (GNU only, does **not** imply `-v`) – same as `-t` but without `-v`.
- [ ] Process one or more files given as command‑line arguments.
- [ ] If no file is provided, read from standard input (optional, but encouraged).
- [ ] Handle errors gracefully (missing files, permission issues) by printing an error message to stderr and continuing with the next file.

### `s21_grep` (basic: flags `-e`, `-i`, `-v`, `-c`, `-l`, `-n`)
- [ ] `-e pattern` – use pattern as the search expression (may be given multiple times).
- [ ] `-i` – ignore case distinctions.
- [ ] `-v` – invert the match (select non‑matching lines).
- [ ] `-c` – print only a count of matching lines per file.
- [ ] `-l` – print only names of files containing at least one match.
- [ ] `-n` – prefix each output line with its line number.
- [ ] Support searching in multiple files.
- [ ] If no file is given, read from standard input (optional).
- [ ] Use **only** `regex.h` (POSIX) or `pcre2.h` for regular expressions.

### `s21_grep` (bonus – additional flags)
- [ ] `-h` – suppress file name prefixes on output.
- [ ] `-s` – suppress error messages about nonexistent or unreadable files.
- [ ] `-f file` – obtain patterns from a file, one pattern per line.
- [ ] `-o` – print only the matching part of a matching line (each match on a separate line).
- [ ] Support arbitrary combinations of flags (e.g., `-iv`, `-in`).

## Non‑functional Requirements
- Language: C11 (`-std=c11`).
- Compiler: `gcc`.
- Code style: Google C++ style guide (applied to C code).
- No deprecated functions (e.g., `gets`, `strcpy` without bounds checking).
- The build system must use `Makefile` with targets:
  - `all` – build both utilities.
  - `s21_cat` – build only `s21_cat`.
  - `s21_grep` – build only `s21_grep`.
  - `clean` – remove object files and executables.
  - `test` – run integration tests (compare with busybox `cat`/`grep`).
- Integration tests must cover all flag combinations and edge cases.
- Reusable code (e.g., line reading, string utilities) must be placed in `src/common/` and linked statically.
- Error message text is not specified – any reasonable message is acceptable.
- The program must return an appropriate exit code (0 on success, non‑zero on error).

---

ARCHITECTURE.md
# Architecture of `s21_cat` and `s21_grep`

## Directory Structure
```
src/
├── cat/
│   ├── s21_cat.c          # main + cat‑specific logic
│   ├── cat_parser.c       # flag parsing for cat
│   ├── cat_processor.c    # line processing (squeeze, numbering, etc.)
│   └── cat.h
├── grep/
│   ├── s21_grep.c         # main + grep‑specific logic
│   ├── grep_parser.c      # flag parsing (incl. -e, -f, combinations)
│   ├── grep_matcher.c     # regex matching, line counting
│   ├── grep_output.c      # formatting results (-c, -l, -n, -o, -h)
│   └── grep.h
└── common/
    ├── file_reader.c      # fopen, fclose, line‑by‑line reading (getline)
    ├── string_utils.c     # safe string functions, trim, etc.
    ├── error_handler.c    # uniform error printing (with optional -s suppression)
    └── common.h
```

## Module Description

### Common modules
- **file_reader** – provides `open_file(const char *path)`, `read_line(FILE *f, char **line, size_t *len)`, `close_file(FILE *f)`. Uses POSIX `getline` for dynamic line length.
- **string_utils** – helper functions: `starts_with`, `contains`, `replace_tabs`, `is_blank_line`, etc.
- **error_handler** – `log_error(const char *prog_name, const char *file_name, const char *message, int suppress)`. If `suppress` is set (for `grep -s`), errors are not printed.

### `s21_cat` modules
- **cat_parser** – parses `argc`/`argv` into a `CatFlags` struct:
  ```c
  typedef struct {
      int b;  // -b
      int e;  // -e (implies v)
      int E;  // -E (no v)
      int n;  // -n
      int s;  // -s
      int t;  // -t (implies v)
      int T;  // -T (no v)
      int v;  // -v (enabled by -e or -t)
  } CatFlags;
  ```
  Supports both short and GNU long options (e.g., `--number-nonblank`).
- **cat_processor** – reads a file line by line, applies transformations:
  - Squeeze blank lines (`-s`): skip consecutive lines that are empty (only `\n` or `\r\n`).
  - Line numbering: `-b` counts non‑empty lines, `-n` counts all lines.
  - End‑of‑line marking (`-e`/`-E`): append `$` before newline.
  - Tab display (`-t`/`-T`): replace `\t` with `^I`.
  - Non‑printing character display (`-v`): encode control chars as `^X`, delete `0x7f` as `^?`, etc. (POSIX rules).
- **s21_cat.c** – orchestrates parsing, then for each file calls the processor.

### `s21_grep` modules
- **grep_parser** – parses flags and patterns:
  ```c
  typedef struct {
      char **patterns;      // array of regex patterns
      int pattern_count;
      int ignore_case;      // -i
      int invert;           // -v
      int count_only;       // -c
      int files_with_matches; // -l
      int line_number;      // -n
      int no_filename;      // -h
      int suppress_errors;  // -s
      int only_matching;    // -o
      char *pattern_file;   // -f
  } GrepFlags;
  ```
  Supports multiple `-e` and `-f`; patterns are collected into a single list.
- **grep_matcher** – compiles patterns (using `regcomp` with `REG_EXTENDED` and optionally `REG_ICASE`). Implements matching for each line:
  ```c
  int match_line(const char *line, regex_t **regexes, int n, int invert);
  ```
  For `-o`, returns a list of match offsets and lengths.
- **grep_output** – formats output according to flag combination:
  - `-c`: print file name + count (or just count if one file and no `-H`? careful with `-h`).
  - `-l`: print file name if any match found, then skip to next file.
  - `-n`: prefix line number.
  - `-h`: suppress file name prefixes even when multiple files.
  - `-o`: print each match on a separate line (with file name and line number if specified).
- **s21_grep.c** – main loop: for each file, open, iterate lines, collect results, call output.

## Build System (Makefile)
- Located in `src/` (or `src/cat/` and `src/grep/` each have their own Makefile? Requirement says "source code, headers, make files must be in `src/cat/` and `src/grep/` respectively". Therefore we need two separate Makefiles, or a top‑level Makefile that calls them. Simpler: one Makefile in `src/` that builds both, but to satisfy the requirement we can place a Makefile inside each directory that builds its own target. We'll propose:
  - `src/cat/Makefile` – builds `s21_cat`.
  - `src/grep/Makefile` – builds `s21_grep`.
  - A top‑level `Makefile` in `src/` that calls both (optional).
- Compilation flags: `-std=c11 -Wall -Wextra -Werror -pedantic`.
- Linking: each utility links with `libcommon.a` built from `src/common/`.
- Pattern for `grep`: use `-lregex` (POSIX). For PCRE2, add `-lpcre2-8` if chosen.

## Testing Strategy
- Integration tests implemented in Bash or Python, located in `tests/` directory.
- Compare outputs of `s21_cat` and `busybox cat` (or `/bin/cat` on Alpine) for a suite of input files (empty, single line, many lines, Windows line endings, binary files).
- Same for `grep` against `busybox grep`.
- Use `diff` or custom test harness; exit with 0 if all tests pass.
- Target `make test` runs all tests.

---

CURRENT_TASKS.md
# Current Tasks – Implementation Checklist

- [ ] **Module common/file_reader** – implement `open_file`, `close_file`, and safe line reading using `getline`.
- [ ] **Module common/string_utils** – implement helper functions: `is_blank_line`, `replace_char`, `append_char`, `encode_control_char` (for cat `-v`), `match_regex_substrings` (for grep `-o`).
- [ ] **Module common/error_handler** – implement `log_error` that respects the suppress flag.
- [ ] **Module cat/parser** – parse command‑line arguments for `s21_cat`, support all required flags.
- [ ] **Module cat/processor** – implement line processing: squeeze blanks, numbering, tab replacement, end‑of‑line marker, non‑printing characters.
- [ ] **Module cat/s21_cat** – main function, iterate over files, use processor to print transformed lines.
- [ ] **Module grep/parser** – parse arguments for `s21_grep`; handle `-e`, `-f`, and flag combinations (e.g., `-iv`). Build pattern list.
- [ ] **Module grep/matcher** – compile regex patterns using POSIX regex; implement matching with `-i`, `-v`, and support for `-o` (multiple matches per line).
- [ ] **Module grep/output** – implement formatting for `-c`, `-l`, `-n`, `-h`, `-o` and their combinations.
- [ ] **Module grep/s21_grep** – main orchestration: parse flags, for each file walk lines, collect matches, invoke output module.
- [ ] **Makefile (src/cat/Makefile)** – build rules for `s21_cat`, linking with `libcommon.a`.
- [ ] **Makefile (src/grep/Makefile)** – build rules for `s21_grep`, linking with `libcommon.a` and `-lregex`.
- [ ] **Integration tests** – scripts comparing `s21_cat` vs `cat` (busybox) for all flags and edge cases.
- [ ] **Integration tests for grep** – scripts comparing `s21_grep` vs `grep` (busybox) for basic and bonus flags.
- [ ] **Documentation** – brief `README.md` describing build and test steps.

@@FILES_COMPLETE@@