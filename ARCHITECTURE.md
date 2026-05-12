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
├── test.sh                # Test suite (optional)
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
  - `file_read_line()`: Read line using getline
  - `file_close_safe()`: Close file (skip stdin)
  - `file_is_stdin()`: Check if FILE* is stdin

### s21_cat Utility
**Supported flags**: -b, -e, -E, -n, -s, -t, -T, -v

**Implementation details**:
- Processes one or more files sequentially
- Handles stdin when no files or "-" specified
- Implements line numbering with proper handling of empty lines
- Squeezes consecutive empty lines when -s specified
- Displays non-printing characters when -v, -t, -e specified

### s21_grep Utility
**Supported flags**: -e, -i, -v, -c, -l, -n, -h, -s, -f, -o

**Implementation details**:
- Uses POSIX regex (regcomp, regexec)
- Supports multiple patterns via -e or -f
- Inverts matching with -v
- Counts matches with -c
- Shows only matching parts with -o
- Handles multiple files with appropriate filename prefixes

## Build System

### Makefile Targets
- `all` or default: Build both utilities
- `s21_cat`: Build only cat utility
- `s21_grep`: Build only grep utility
- `clean`: Remove object files and executables
- `test`: Run test suite (`test.sh`)
- `gcov_report` / `coverage`: Generate code coverage report (requires `lcov` and `genhtml`)
- `valgrind`: Run test suite under Valgrind to detect memory leaks
- `check`: Run static analysis (`cppcheck`) and code style check (`clang-format`)
- `review`: Display a manual code review checklist
- `install`: Install binaries to `/usr/local/bin`
- `uninstall`: Remove installed binaries
- `dvi`: Display documentation info
- `dist`: Create distribution archive

### Quality Gates (must pass before merge)
1. `make` – no compilation warnings.
2. `make test` – all integration tests pass.
3. `make valgrind` – zero memory leaks.
4. `make check` – no static analysis warnings, code formatting compliant.
5. `make gcov_report` – line coverage ≥80% (view `gcov_report/index.html`).
- `s21_cat`: Build only cat utility
- `s21_grep`: Build only grep utility
- `clean`: Remove object files and executables
- `test`: Run test suite
- `gcov_report`: Generate code coverage report
- `install`: Install binaries to /usr/local/bin
- `uninstall`: Remove installed binaries
- `dvi`: Display documentation info
- `dist`: Create distribution archive

### Compilation Flags
- `-Wall -Werror -Wextra`: Enable all warnings as errors
- `-std=c11`: C11 standard compliance
- `-g`: Debug information
- `-fprofile-arcs -ftest-coverage`: Coverage analysis (for gcov_report)

## Coding Standards
- **Language**: C11
- **Style**: Google C Style Guide
- **No unsafe functions**: No gets, strcpy, sprintf, etc.
- **Memory management**: All dynamic memory must be freed
- **Error handling**: Functions return error codes, handle edge cases
- **Warnings**: Code compiles without warnings with -Wall -Werror -Wextra

## Dependencies
- **Build**: gcc, make
- **Runtime**: Standard C library
- **Testing**: bash (for test.sh), lcov/genhtml (for coverage report)

## Error Handling
- Invalid arguments: Print usage and exit with code 1
- File open errors: Print error unless -s (silent) mode
- Memory allocation failures: Clean up and exit appropriately
- Regex compilation errors: Report and exit

## Future Extensions
- Add more cat flags: -A, -u (if needed)
- Add grep flag: -w (word regexp)
- Improved performance for large files
- Thread support for parallel file processing