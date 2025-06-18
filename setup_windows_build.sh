#!/bin/bash

# Windows Cross-Compilation Setup Script
# This script sets up the environment for cross-compiling your compiler to Windows

set -e  # Exit on any error

echo "=== Windows Cross-Compilation Setup ==="
echo "This script will help you build your compiler for Windows"
echo ""

# Check if running on Linux
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "Error: This script is designed for Linux systems"
    exit 1
fi

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Install MinGW-w64 if not present
echo "1. Checking MinGW-w64 cross-compiler..."
if ! command_exists x86_64-w64-mingw32-gcc; then
    echo "Installing MinGW-w64..."
    sudo apt update
    sudo apt install -y gcc-mingw-w64 g++-mingw-w64
else
    echo "✓ MinGW-w64 already installed"
fi

# Install Flex if not present
echo ""
echo "2. Checking Flex..."
if ! command_exists flex; then
    echo "Installing Flex..."
    sudo apt install -y flex
else
    echo "✓ Flex already installed"
fi

# Install Gperf if not present
echo ""
echo "3. Checking Gperf..."
if ! command_exists gperf; then
    echo "Installing Gperf..."
    sudo apt install -y gperf
else
    echo "✓ Gperf already installed"
fi

# Check LLVM installation
echo ""
echo "4. Checking LLVM..."
if command_exists llvm-config; then
    LLVM_VERSION=$(llvm-config --version)
    echo "✓ LLVM version $LLVM_VERSION found"
    
    # Try to find Windows LLVM libraries
    LLVM_LIB_PATH=$(llvm-config --libdir)
    echo "LLVM libraries found at: $LLVM_LIB_PATH"
else
    echo "Warning: LLVM not found. You may need to install it:"
    echo "  sudo apt install llvm-dev libllvm-ocaml-dev"
fi

echo ""
echo "5. Setting up Windows build environment..."

# Create a simple build script
cat > build_windows.sh << 'EOF'
#!/bin/bash
echo "Building compiler for Windows..."
cd src
make -f Makefile.windows clean
make -f Makefile.windows
echo ""
echo "Build complete! Check for compiler.exe in the src directory."
EOF

chmod +x build_windows.sh

echo "✓ Created build_windows.sh script"

echo ""
echo "=== Setup Complete ==="
echo ""
echo "To build your compiler for Windows:"
echo "  ./build_windows.sh"
echo ""
echo "Or manually:"
echo "  cd src"
echo "  make -f Makefile.windows"
echo ""
echo "Note: If you encounter LLVM linking issues, you may need to:"
echo "1. Download LLVM Windows binaries from https://github.com/llvm/llvm-project/releases"
echo "2. Extract them and update LLVM_ROOT in Makefile.windows"
echo "3. Or consider building a statically linked version"