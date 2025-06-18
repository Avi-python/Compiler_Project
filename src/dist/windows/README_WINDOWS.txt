# Windows Deployment Guide

Your compiler has been built with full LLVM code generation support for Windows. Here are your options for using it:

## Distribution Contents

- `compiler.exe` - Your compiler executable with full LLVM support
- `LLVM-C.dll` - Required LLVM runtime library
- `README_WINDOWS.txt` - This file

## Compilation Options

### Option 1: Generate LLVM IR Only (Recommended for distribution)
```cmd
compiler.exe -S program.c
```
This generates `program.ll` (LLVM IR) which can be compiled later with any LLVM-compatible compiler.

### Option 2: Use with External Clang (Full compilation)
To compile LLVM IR to executables, you need to install Clang on the target Windows machine:

1. **Download LLVM/Clang for Windows:**
   - Visit: https://releases.llvm.org/
   - Download and install LLVM for Windows
   - Add LLVM bin directory to your PATH

2. **Use the compiler normally:**
   ```cmd
   compiler.exe program.c          # Creates executable
   compiler.exe -c program.c       # Creates object file
   ```

### Option 3: Use with Visual Studio (Alternative)
If you have Visual Studio installed, the compiler will attempt to use `cl.exe` as a fallback.

## Recommended Workflow

For maximum compatibility, we recommend this two-step approach:

1. **Generate LLVM IR:**
   ```cmd
   compiler.exe -S program.c
   ```

2. **Compile with Clang (if available):**
   ```cmd
   clang program.ll -o program.exe
   ```

## Error Messages

If you see "Failed to find 'clang' in PATH", it means:
- Clang is not installed on the system
- LLVM bin directory is not in the PATH
- Use the `-S` flag to generate .ll files only

## Self-Contained Alternative

For a completely self-contained solution, the compiler could be enhanced to use the LLVM-C API directly for object file generation, eliminating the need for external tools entirely.

