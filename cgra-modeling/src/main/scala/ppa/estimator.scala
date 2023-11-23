package ppa

import chisel3.util._
import scala.collection.mutable
import scala.math._
import op._

object estimator {

def get_area(jsonMap: Map[String,Any]) : Double = {
  var module_areas: mutable.Map[Int, Double] = mutable.Map()
  var area = 0.0
  var ib_area = 0.0
  var ob_area = 0.0
  var gib_area = 0.0
  var pe_area = 0.0
  val rows = jsonMap("num_row").asInstanceOf[Int]
  val cols = jsonMap("num_colum").asInstanceOf[Int]
  val numPE = rows * cols
  for(m <- jsonMap("sub_modules").asInstanceOf[List[Any]]){
    val module = m.asInstanceOf[Map[String, Any]]
    val attributes = module("attributes").asInstanceOf[Map[String, Any]]
    val id = module("id").asInstanceOf[Int]
    val module_type = module("type").asInstanceOf[String]
    var module_area = 0.0
    var cfgwidth = 0
    var muxnum = 0
    //Muxn area
    for(i <- attributes("instances").asInstanceOf[List[Any]]){
      val subinstance = i.asInstanceOf[Map[String, Any]]
      if(subinstance("type") == "Muxn"){
        val subinstance_id = subinstance("id")
        var innum = 0
        for((k, v) <- attributes("connections").asInstanceOf[Map[String, Any]]){
          val connection = v.asInstanceOf[List[Any]]
          val dstid = connection(3).asInstanceOf[Int]
          if(dstid == subinstance_id){
            innum += 1
          }
        }
        val muxcfgwidth = log2Ceil(innum)
        cfgwidth += muxcfgwidth
        val rate = if(module_type == "GIB" && innum > 4 && numPE > 32) { 1 - area_par.reduce_rate } else if (module_type == "GIB" && innum == 4) { 1 - 0.5 * area_par.reduce_rate } else { 1 }
        //val muxarea = if(module_type == "GIB") { area_par.area_mux32_map(innum) * (1 - area_par.reduce_rate) } else { area_par.area_mux32_map(innum) }
        val muxarea = area_par.area_mux32_map(innum) * rate
        module_area += muxarea
        muxnum += 1
      }
    }
    //GIB track reged
    if(module_type == "GIB") { module_area += muxnum * area_par.area_regnxt32 }
    //Other hardware unit area
    if(module_type == "GPE"){
      val dataWidth = attributes("data_width").asInstanceOf[Int]
      val operations = attributes("operations").asInstanceOf[List[String]]
      val max_delay = attributes("max_delay").asInstanceOf[Int]
      val operandnum = operations.map(OPC.withName(_)).map(OpInfo.getOperandNum(_)).max
      val delaycfgwidth = operandnum * log2Ceil(max_delay + 1)
      val constcfgwidth = dataWidth // constant
      val alucfgwidth = log2Ceil(OPC.numOPC) // ALU Config width
      val rfcfgwidth = 0  // RF
      cfgwidth += delaycfgwidth + constcfgwidth + alucfgwidth + rfcfgwidth
      val delay_area = area_par.area_delay32_map(operandnum)(max_delay)
      val alu_rate = if(operations.size > 2 && operations.contains("MUL") == true && numPE > 32) { 0.75 } else if (operations.size > 1) { 0.9 } else { 1 }
      val alu_area = (operations.foldLeft(0.0){(a,b) => a + area_par.area_alu32_map(b)}) * alu_rate
      val constarea = area_par.area_const32
      val rfarea = area_par.area_rf32
      module_area += delay_area + alu_area + constarea + rfarea
    }
    val cfg_area = cfgwidth * area_par.area_cfgpre32
    module_area += cfg_area
    module_areas(id) = module_area
  }
  for(item <- jsonMap("instances").asInstanceOf[List[Any]]){
    val instance = item.asInstanceOf[Map[String, Any]]
    if(instance("type") != "This"){
      val module_id = instance("module_id").asInstanceOf[Int]
      area += module_areas(module_id)
      if(instance("type") == "GPE"){
        pe_area += module_areas(module_id)
      }
      else if(instance("type") == "GIB"){
        gib_area += module_areas(module_id)
      }
      else if(instance("type") == "IB"){
        ib_area += module_areas(module_id)
      }
      else if(instance("type") == "OB"){
        ob_area += module_areas(module_id)
      }
    }
  }
  val cfgDataWidth = jsonMap("cfg_data_width").asInstanceOf[Int]
  val cfgAddrWidth = jsonMap("cfg_addr_width").asInstanceOf[Int]
  val cfgregnum = 2 * rows + 4
  val cfgreg_area = cfgregnum * (1 + cfgAddrWidth + cfgDataWidth) * area_par.area_rf32 / 32
  area += cfgreg_area
  //println("IB area: " + ib_area.formatted("%.2f"))
  //println("OB area: " + ob_area.formatted("%.2f"))
  //println("GPE area: " + pe_area.formatted("%.2f"))
  //println("GIB area: " + gib_area.formatted("%.2f"))
  area
}

}