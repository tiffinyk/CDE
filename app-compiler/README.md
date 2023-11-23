App-Compiler
=======================

Frontend application compiler that generates CDFG from C code based on LLVM


## Getting Started

### Dependencies

1. LLVM-15 with Polly included

2. CMake

3. C++-14

4. GraphViz

### Build

First, change to the app-compiler directory.

In the CMakeList.txt file, you should change the LLVM_INCLUDE_DIRS in line 8 and the LLVM_LIBRARY_DIRS in line 9 to your actual LLVM directories.

After that, you can use the two following commands to compile the app-compiler:

"$cmake -B build"

"$cmake --build build"

### Use

1. Change to the "../applications/array_add/" directory.

2. Then, you can use the following command to generate the DFG DOT file (affine.dot) from the C code:

"$make"

3. After that, you can use the GraphViz tool's following command to convert the DFG DOT file to JSON format:

"$dot -Tdot_json affine.dot -o array_add.json"