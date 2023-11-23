CGRA Development Environment (CDE)
=======================

## Getting Started

### Directory Structure

#### CDE Main Program: DSE, DFG mapping, and GUI

In the ./src (C++ source files) folder and the ./include (C++ header files) folder

#### Application Compiling Frontend: Generate DFG from C code

In the ./app-compiler folder

#### Architecture Modeling

In the ./cgra-modeling folder

### Dependencies

#### CMake

#### C++ 17

#### Qt 5.14.2

#### Boost 1.74.0

#### JDK 8 or newer (for architecture modeling tool)

#### SBT (for architecture modeling tool)

#### LLVM 15 with Polly included (for application comliling frontend)

#### GraphViz (to convert the DFG file's format)

### Compile and run the CDE

First, change the directory of set(CMAKE_PREFIX_PATH "(directory)") in CMakeLists.txt's line 5 to your actural Qt-cmake directory.

Then, use the following two command to compile the CDE:

"$cmake -B build"

"$cmake --build build"

Windows users can use the Visual Studio to build the project and compile it.

After compilation, click the "CDE" executable file to open it.

### Design Space Exploration

This CDE version can explore the GIB-based CGRA architectures.

1. Click the "File->Open->Open Project" in the menu bar, then open the CDE project file "dse12.cpf" in the "resources/" folder. The CDE will load the operation file and the DFG files of the project.

2. In the "DFGs" tab of "Circuit Information" bar (on the left of the CDE GUI), you can see the loaded DFG files. Select the DFGs for DSE, then click the "Operation->Generate Arch" in the menu bar. CDE will explore the CGRA architecture fastly and display the explored architecture.

3. After DSE, the mapped DFGs will be listed in the "Netlists" tab of "Circuit Information" bar. You can select only one mapped DFG to see its mapping result, or click "File->Save" to save the mapping result.

4. You can click the "File->Save Arch" to save the explored architecture as CGRA ADG file.

5. You can click the "File->Close Project" to close all the files and open new files.

### Application DFG Mapping

CDE version can map the application DFG on the existing CGRA ADG.

1. Click the "File->Open->Open Project" in the menu bar, then open the CDE project file "mapping.cpf" in the "resources/" folder. The CDE will load the ADG file, the operation file, and the DFG file of the project. The CGRA architecture will be displayed.

2. Click the "Operation->Map Design" in the menu bar. CDE will map the DFG to the ADG and display the mapping result.

3. After DFG mapping, the mapped DFG will be listed in the "Netlists" tab of "Circuit Information" bar. You can click "File->Save As" to save the mapping result.

### Display the Architectures and Mapped DFGs

1. Click the "File->Open->Load Arch" and open the CGRA ADG file. The CGRA architecture will be displayed.

2. Click the "File->Open->Open Operation File" and open the CGRA operation file "operations.json" in the "resources/operations/" folder.

3. Click the "File->Open->Open Netlist" and open the saved mapping result. Then the mapping result will be displayed. It will be listed in the "Netlists" tab of "Circuit Information" bar. WARNING: The mapping result must correspond to the opened ADG.
