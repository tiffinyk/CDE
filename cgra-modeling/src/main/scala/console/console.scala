package modeling

import spec._
import op._
import dsa._

// Generate ADG and Verilog from design parameters.
object ManualDesign extends App{
  var loadSpec : Boolean = true
  var dumpOperations : Boolean = true
  var dumpIR : Boolean = true
  var genVerilog : Boolean = true
  var getArea : Boolean = true

  if(loadSpec){
    val jsonFile = "../resources/designs/cgra_spec.json"
    CGRASpec.loadSpec(jsonFile)
  }
  if(dumpOperations){
    val jsonFile = "../resources/operations/cgra_operations.json"
    OpInfo.dumpOpInfo(jsonFile)
  }

  val outFile = "../resources/ADGs/cgra_adg.json"
  if(genVerilog){
    (new chisel3.stage.ChiselStage).emitVerilog(new CGRA(CGRASpec.attrs, getArea, dumpIR, outFile), Array("--target-dir", "../resources/hardwares"))
  }else{ // not emit verilog to speedup
    (new chisel3.stage.ChiselStage).emitChirrtl(new CGRA(CGRASpec.attrs, getArea, dumpIR, outFile), Array("--target-dir", "../resources/hardwares"))
  }
}

// Generate Verilog from ADG directly and estimate the hardware area.
object ProcessADG extends App{
  var getArea : Boolean = true
  var genVerilog : Boolean = true
  val filename = "../resources/ADGs/test11.json"

  val start = System.currentTimeMillis()
  val info = new arch_info(filename, getArea)
  if(genVerilog){
    (new chisel3.stage.ChiselStage).emitVerilog(new arch(info.get_attrs), Array("--target-dir", "../resources/hardwares"))
  }
  val time = System.currentTimeMillis() - start
  println("running time: " + time + "ms.")
}
