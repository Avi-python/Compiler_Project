#!/bin/bash
echo "Building compiler for Windows with full LLVM support..."
echo "Using Windows LLVM binaries from tools/windows-llvm/"
cd src
make -f Makefile.windows clean
make -f Makefile.windows
echo ""
echo "Build complete! Check for compiler.exe with full code generation in the src directory."
echo "Note: When deploying, make sure to include LLVM-C.dll alongside the executable."
