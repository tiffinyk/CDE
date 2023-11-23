CGRA-Modeling
=======================

CGRA modeling and generation tool with Chisel

## Getting Started

### Dependencies

#### JDK 8 or newer

#### SBT

### Build and run

First, change to the directory of the modeling tool:

"$cd cgra-modeling"

Then, use the following sbt command to enter the console of the modeling tool:

"$sbt run"

After that, input the number "1" to generate the ADG and Verilog file from design parameters. (modeling.ManualDesign)

Or input the number "2" to estimate the hardware area generate the Verilog file from the existing ADG file. (modeling.ProcessADG)

#### Design Parameter files

This type of file is used for manual design (selection "1" in the modeling tool console): ../resources/designs/cgra_spec.json

#### Output files

Generated ADG file: ../resources/ADGs/cgra_adg.json

Generated operation set file: ../resources/operations/cgra_operations.json

Generated Verilog file: ../resources/hardwares/CGRA.v (by modeling.ManualDesign) or ../resources/hardwares/arch.v (by modeling.ProcessADG)
