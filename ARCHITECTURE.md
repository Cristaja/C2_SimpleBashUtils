# Architecture Documentation for C2_SimpleBashUtils

## Project Overview
Implementation of simplified versions of standard Unix utilities `cat` and `grep` in C.

## Project Structure
```
.
├── cat.c                  # s21_cat utility implementation
├── grep.c                 # s21_grep utility implementation
├── common/                # Shared modules
│   ├── arg_parser.h       # Argument parsing interface
│   ├── arg_parser.c       # Argument parsing implementation
│   ├── file_utils.h       # File I/O utilities interface
│   └── file_utils.c       # File I/O utilities implementation
├── Makefile               # Build configuration
├── test.sh                # Test suite
└── ARCHITECTURE.md        # This file
```

## Modules

### Common Module
Shared utilities used by both `s21_cat` and `s21_grep`.

#### arg_parser
- **Purpose**: Parse command-line arguments for both utilities
- **Functions**:
  - `parse_cat_args()`: Parse cat-specific options and file arguments
  - `parse_grep_args()`: Parse grep-specific options, patterns, and file arguments
  - `free_parsed_args()`: Free dynamically allocated argument arrays

#### file_utils
- **Purpose**: Safe file operations with stdin handling
- **Functions**:
  - `file_open_safe()`: Open file or return stdin for "-"
  - `file_close_safe()`: Close file (skip stdin)

### s21_cat Utility
**Supported flags**: -b, -e, -E, -n, -s, -t, -T, -v

### s21_grep Utility
**Supported flags**: -e, -i, -v, -c, -l, -n, -h, -s, -f, -o

## Build System

### Makefile Targets
- `all`: Build both utilities
- `s21_cat`: Build only cat utility
- `s21_grep`: Build only grep utility
- `clean`: Remove object files and executables
- `test`: Run test suite
- `gcov_report`: Generate code coverage report
- `valgrind`: Run memory leak check
- `check`: Run static analysis and style check
- `install`: Install binaries to /usr/local/bin
- `uninstall`: Remove installed binaries
- `dvi`: Display documentation info
- `dist`: Create distribution archive

## Coding Standards
- **Language**: C11
- **Style**: Google C Style Guide
- **No unsafe functions**: No gets, strcpy, sprintf, etc.
- **Memory management**: All dynamic memory must be freed
- **Error handling**: Functions return error codes, handle edge cases
- **Compilation**: `-Wall -Werror -Wextra -std=c11 -D_GNU_SOURCE` for POSIX functions