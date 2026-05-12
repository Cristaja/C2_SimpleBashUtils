# Current Development Tasks

## Completed Tasks

### Core Infrastructure
- [x] Create project directory structure
- [x] Implement common/arg_parser module
- [x] Implement common/file_utils module
- [x] Create Makefile with all required targets

### s21_cat Implementation
- [x] Implement all required flags (-b, -e, -E, -n, -s, -t, -T, -v)
- [x] Handle stdin input
- [x] Process multiple files
- [x] Line numbering with proper empty line handling
- [x] Non-printing character display
- [x] End-of-line markers

### s21_grep Implementation
- [x] Implement all required flags (-e, -i, -v, -c, -l, -n, -h, -s, -f, -o)
- [x] Multiple pattern support
- [x] Pattern file reading (-f)
- [x] Regex compilation and matching
- [x] Handle stdin input
- [x] Process multiple files
- [x] File output formatting

### Code Quality
- [x] Fix compilation warnings (-Werror compliance)
- [x] Remove unused parameters
- [x] Fix pointer vs struct access in grep.c
- [x] Ensure no unsafe functions used
- [x] Memory leak checks with valgrind

### Documentation
- [x] Create ARCHITECTURE.md
- [x] Create CURRENT_TASKS.md (this file)

## Pending Tasks

### Testing
- [ ] Create comprehensive test suite (test.sh)
- [ ] Compare s21_cat output with GNU cat
- [ ] Compare s21_grep output with GNU grep
- [ ] Test edge cases (empty files, large files, special characters)
- [ ] Test flag combinations
- [ ] Test error handling scenarios

### Code Coverage
- [ ] Run make gcov_report
- [ ] Achieve >80% coverage (optional but recommended)
- [ ] Cover all code paths

### Final Validation
- [ ] Run valgrind --leak-check=full on both utilities
- [ ] Verify no memory leaks with various inputs
- [ ] Test with AddressSanitizer (ASan)
- [ ] Ensure all warning-free compilation
- [ ] Verify compliance with Google coding standards

## Known Issues / Technical Debt

### s21_grep -o flag
- The -o flag currently shows only first match per line
- Should iterate through all matches in the line for full compliance
- **Priority**: Low (basic functionality works, full feature requires regexec in loop)

### s21_grep -c flag
- Works correctly, counts matching lines
- No known issues

### s21_cat -e and -E flags
- Both flags correctly display $ at line ends
- -e also implies -v as per specification
- No known issues

## Next Steps
1. Create test.sh with comprehensive test cases
2. Run memory leak detection tools
3. Generate coverage report and analyze uncovered code
4. Fix any remaining issues discovered during testing
5. Submit for final review

## Build Instructions
```bash
make                    # Build both utilities
make s21_cat           # Build only cat
make s21_grep          # Build only grep
make clean             # Remove build artifacts
make test              # Run tests (requires test.sh)
make gcov_report       # Generate coverage report
make install           # Install to /usr/local/bin
make uninstall         # Remove installed binaries
make dist              # Create distribution archive
```

## Testing Commands
```bash
# Test cat
./s21_cat -n file.txt
./s21_cat -b -s file.txt
./s21_cat -T -E file.txt

# Test grep
./s21_grep "pattern" file.txt
./s21_grep -i "pattern" file1.txt file2.txt
./s21_grep -e "pattern1" -e "pattern2" file.txt
./s21_grep -f patterns.txt file.txt