# CUTLASS CuTe compatibility tests

This directory imports NVIDIA CUTLASS's complete `test/unit/cute` test tree.
The current snapshot is taken from CUTLASS `main` at commit
`f94ec46f4f63f96003d6cfdf2014731e7672c281`.

The imported source files retain the upstream test names, assertions, directory
layout, and CMake files. No MiniCuTe implementation or build target is changed
by this import. The `core` tests are host-side in CUTLASS, while the other
subdirectories contain CUDA or architecture-specific tests and require the
corresponding CUTLASS/CUDA test runner.

Source: <https://github.com/NVIDIA/cutlass/tree/main/test/unit/cute/core>
