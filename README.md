# Sample C++ Project with Dynamic Analysis & Sanitizer Verification

This project demonstrates a 100% sanitizer-clean C++17 codebase verified through AddressSanitizer (ASan), UndefinedBehaviorSanitizer (UBSan), ThreadSanitizer (TSan), Valgrind Memcheck, Helgrind, DRD, Cppcheck, and Clang-Tidy.

## Automated Verification Pipeline
Every push to `main` triggers a GitHub Actions pipeline (`.github/workflows/sanitizer-pipeline.yml`) that:
1. Compiles binaries with GCC & Clang instrumentation (`-fsanitize=address,undefined,thread`).
2. Runs dynamic memory and data race analyses.
3. Executes static analysis with `cppcheck` and `clang-tidy`.
4. Generates and cross-checks the validation report CSV.
5. Packages all raw log files into a downloadable zip artifact.
