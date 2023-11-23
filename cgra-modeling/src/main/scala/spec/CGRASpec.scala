 package spec
 // Architecture Specification

 import chisel3._
 import chisel3.util._
 import ppa._
 import scala.math._
 import scala.collection.mutable
 import scala.collection.mutable.ListBuffer
 import common.MacroVar.{NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST}
 import dsa.template.{GibParam, GpeParam}
 import dsa._
 import ir._
 import op._

 // GPE Spec to support heterogeneous GPEs
 class GpeSpec(num_rf_reg_ : Int = 1, max_delay_ :Int = 4,
               operations_ : ListBuffer[String] = ListBuffer( "PASS", "ADD", "SUB", "MUL", "AND", "OR", "XOR"),
               fromdir_ : List[Int] =  List(NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST),
               todir_ : List[Int] = List(NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST)){
   var num_rf_reg =  num_rf_reg_  // number of registers in Regfile
   var max_delay = max_delay_     // max delay cycles of the DelayPipe
   var operations = operations_   // supported operations
   var from_dir = fromdir_        // which directions the GPE inputs are from
   var to_dir = todir_            // which directions the GPE outputs are to
 }

 object GpeSpec {
   def apply(num_rf_reg_ : Int = 1, max_delay_ :Int = 4,
             operations_ :ListBuffer[String] = ListBuffer( "ADD"),
             fromdir_ : List[Int] =  List(NORTHWEST,NORTHEAST, SOUTHWEST, SOUTHEAST),
             todir_ : List[Int] =  List(NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST)) = {
     new GpeSpec(num_rf_reg_, max_delay_, operations_, fromdir_, todir_)
   }
//   def apply(attr : mutable.Map[String, Any]) = {
//     val num_rf_reg_ = attr("num_rf_reg").asInstanceOf[Int]
//     val max_delay_ = attr("max_delay").asInstanceOf[Int]
//     val operations_ = attr("operations").asInstanceOf[List[String]].to[ListBuffer]
//     val fromdir_ = attr("from_dir").asInstanceOf[List[Int]]
//     val todir_ = attr("to_dir").asInstanceOf[List[Int]]
//     new GpeSpec(num_rf_reg_, max_delay_, operations_, fromdir_, todir_)
//   }
 }


 // GIB Spec to support heterogeneous GIBs
 class GibSpec(diagIOPinConnect : Boolean = true , fclist_ : List[Int] = List(2, 4, 4)){
   var diag_iopin_connect = diagIOPinConnect // if support diagonal connections between OPins and IPins
   var fclist = fclist_ // num_itrack_per_ipin, num_otrack_per_opin, num_ipin_per_opin
   // "num_itrack_per_ipin" : ipin-itrack connection flexibility, connected track number
   // "num_otrack_per_opin" : opin-otrack connection flexibility, connected track number
   // "num_ipin_per_opin"   : opin-ipin  connection flexibility, connected ipin number
 }

 object GibSpec {
   def apply(diagIOPinConnect : Boolean = true , fclist_ : List[Int] = List(2, 4, 4)) = {
     new GibSpec(diagIOPinConnect, fclist_)
   }
//   def apply(attr : mutable.Map[String, Any]) = {
//     val diagIOPinConnect = attr("diag_iopin_connect").asInstanceOf[Boolean]
//     val fclist_ = attr("fclist").asInstanceOf[List[Int]]
//     new GibSpec(diagIOPinConnect, fclist_)
//   }
 }

 // CGRA Specification
 object CGRASpec{
   val connect_flexibility = mutable.Map(
     "num_itrack_per_ipin" -> 2, // ipin number = 2
     "num_otrack_per_opin" -> 4, // opin number = 1
     "num_ipin_per_opin"   -> 4
   )

   val attrs: mutable.Map[String, Any] = mutable.Map(
     "num_row" -> 4,
     "num_colum" -> 4,
     "data_width" -> 32,
     "cfg_data_width" -> 32,
     "cfg_addr_width" -> 12,
     "cfg_blk_offset" -> 2,
     "num_rf_reg" -> 1,
     "operations" -> ListBuffer("PASS", "ADD", "SUB", "MUL", "AND", "OR", "XOR"),
     "max_delay" -> 4,
     "gpe_in_from_dir" -> List(NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST),
     "gpe_out_to_dir" -> List(NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST),
     "num_track" ->1,
     "track_reged_mode" -> 1,
     "connect_flexibility" -> connect_flexibility,
     "diag_iopin_connect" -> true,
     "num_output_ib" -> 1,
     "num_input_ob" -> 2,
     "num_input_ib" -> 1,
     "num_output_ob" -> 1
   )
   //     attrs += ("gpes" -> gpes_spec)
   //     attrs += ("gibs" -> gibs_spec)
   // set default values from attr
   // the attributes in attrs are used as default values
   def setDefaultGpesSpec(): Unit = {
     val gpes_spec = ListBuffer[ListBuffer[GpeSpec]]()
     for(i <- 0 until attrs("num_row").asInstanceOf[Int]){
       gpes_spec.append(new ListBuffer[GpeSpec])
       for( j <- 0 until attrs("num_colum").asInstanceOf[Int]){
         val num_rf_reg = attrs("num_rf_reg").asInstanceOf[Int]
         val max_delay = attrs("max_delay").asInstanceOf[Int]
         val operations = attrs("operations").asInstanceOf[ListBuffer[String]]
         val fromdir = attrs("gpe_in_from_dir").asInstanceOf[List[Int]]
         val todir= attrs("gpe_out_to_dir").asInstanceOf[List[Int]]
         gpes_spec(i).append(GpeSpec(num_rf_reg, max_delay, operations, fromdir, todir))
       }
     }
     attrs("gpes") = gpes_spec
   }

   def setDefaultGibsSpec(): Unit = {
     val gibs_spec = ListBuffer[ListBuffer[GibSpec]]()
     for(i <- 0 to attrs("num_row").asInstanceOf[Int]){
       gibs_spec.append(new ListBuffer[GibSpec])
       for( j <- 0 to attrs("num_colum").asInstanceOf[Int]){
         val diag_iopin_connect = attrs("diag_iopin_connect").asInstanceOf[Boolean]
         val conf = attrs("connect_flexibility").asInstanceOf[mutable.Map[String, Int]]
         val fclist = List(conf("num_itrack_per_ipin"), conf("num_otrack_per_opin"), conf("num_ipin_per_opin"))
         gibs_spec(i).append(GibSpec(diag_iopin_connect, fclist))
       }
     }
     attrs("gibs") = gibs_spec
   }

   setDefaultGpesSpec()
   setDefaultGibsSpec()


   def loadSpec(jsonFile : String): Unit ={
     val jsonMap = IRHandler.loadIR(jsonFile)
     var gpes_spec_update = false
     var gibs_spec_update = false
     for(kv <- jsonMap){
       if(attrs.contains(kv._1)) {
         if (kv._1 == "operations") {
           attrs(kv._1) = kv._2.asInstanceOf[List[String]].to[ListBuffer]
         } else if (kv._1 == "connect_flexibility") {
           attrs(kv._1) = mutable.Map() ++ kv._2.asInstanceOf[Map[String, Int]]
         } else if (kv._1 == "gpe_in_from_dir") {
           attrs(kv._1) = kv._2.asInstanceOf[List[Int]]
         } else if (kv._1 == "gpe_out_to_dir") {
           attrs(kv._1) = kv._2.asInstanceOf[List[Int]]
         } else if (kv._1 == "gpes") {
           gpes_spec_update = true
           val gpe_2d = kv._2.asInstanceOf[List[List[Any]]]
           val gpes_spec = ListBuffer[ListBuffer[GpeSpec]]()
           for (i <- gpe_2d.indices) {
             gpes_spec.append(new ListBuffer[GpeSpec])
             val gpe_1d = gpe_2d(i)
             for (j <- gpe_1d.indices) {
               val gpemap = gpe_1d(j).asInstanceOf[Map[String, Any]]
               val num_rf_reg = gpemap("num_rf_reg").asInstanceOf[Int]
               val max_delay = gpemap("max_delay").asInstanceOf[Int]
               val operations = gpemap("operations").asInstanceOf[List[String]].to[ListBuffer]
               val fromdir = gpemap("from_dir").asInstanceOf[List[Int]]
               val todir = gpemap("to_dir").asInstanceOf[List[Int]]
               gpes_spec(i).append(GpeSpec(num_rf_reg, max_delay, operations, fromdir, todir))
             }
           }
           attrs("gpes") = gpes_spec
         } else if (kv._1 == "gibs") {
           gibs_spec_update = true
           val gib_2d = kv._2.asInstanceOf[List[List[Any]]]
           val gibs_spec = ListBuffer[ListBuffer[GibSpec]]()
           for (i <- gib_2d.indices) {
             gibs_spec.append(new ListBuffer[GibSpec])
             val gib_1d = gib_2d(i)
             for (j <- gib_1d.indices) {
               val gibmap = gib_1d(j).asInstanceOf[Map[String, Any]]
               val diag_iopin_connect = gibmap("diag_iopin_connect").asInstanceOf[Boolean]
               val fclist = gibmap("fclist").asInstanceOf[List[Int]]
               gibs_spec(i).append(GibSpec(diag_iopin_connect, fclist))
             }
           }
           attrs("gibs") = gibs_spec
         } else {
           attrs(kv._1) = kv._2
         }
       }
     }
     if(gpes_spec_update == false){ // set default values
       setDefaultGpesSpec()
     }
     if(gibs_spec_update == false){ // set default values
       setDefaultGibsSpec()
     }
   }

   def dumpSpec(jsonFile : String): Unit={
     IRHandler.dumpIR(attrs, jsonFile)
   }

 }

class arch(jsonMap: Map[String, Any]) extends Module with IR {
  var modules: mutable.Map[Int, Map[String, Any]] = mutable.Map()
  var instances: mutable.Map[Int, hardware] = mutable.Map()
  var pes: mutable.Map[Int, pe] = mutable.Map()
  // CGRA parameters
  val numIn = jsonMap("num_input").asInstanceOf[Int]
  val numOut = jsonMap("num_output").asInstanceOf[Int]
  val rows = jsonMap("num_row").asInstanceOf[Int]
  val cols = jsonMap("num_colum").asInstanceOf[Int]
  val dataWidth = jsonMap("data_width").asInstanceOf[Int]
  val cfgDataWidth = jsonMap("cfg_data_width").asInstanceOf[Int]
  val cfgAddrWidth = jsonMap("cfg_addr_width").asInstanceOf[Int]
  val cfgBlkOffset = jsonMap("cfg_blk_offset").asInstanceOf[Int]
  var pesrcs: ListBuffer[List[Int]] = new ListBuffer[List[Int]]
  var pedsts: ListBuffer[List[Int]] = new ListBuffer[List[Int]]
  var othersrcs: ListBuffer[List[Int]] = new ListBuffer[List[Int]]
  var otherdsts: ListBuffer[List[Int]] = new ListBuffer[List[Int]]

  val io = IO(new Bundle{
    val cfg_en   = Input(Bool())
    val cfg_addr = Input(UInt(cfgAddrWidth.W))
    val cfg_data = Input(UInt(cfgDataWidth.W))
    val en  = Input(Vec(cols, Bool())) // top and buttom row
    val in  = Input(Vec(numIn, UInt(dataWidth.W)))
    val out = Output(Vec(numOut, UInt(dataWidth.W)))
  })

  for(item <- jsonMap("sub_modules").asInstanceOf[List[Any]]){
    val m = item.asInstanceOf[Map[String, Any]]
    val id = m("id").asInstanceOf[Int]
    modules(id) = m
  }

  for(item <- jsonMap("instances").asInstanceOf[List[Any]]){
    val i = item.asInstanceOf[Map[String, Any]]
    if(i("type") == "GPE"){
      val instance_id = i("id").asInstanceOf[Int]
      val module_id = i("module_id").asInstanceOf[Int]
      val cfgBlkIndex = i("cfg_blk_index").asInstanceOf[Int]
      val instance = Module(new pe(modules(module_id), cfgAddrWidth, cfgDataWidth, cfgBlkIndex, cfgBlkOffset))
      pes(instance_id) = instance
      for(i <- 0 until instance.io.in.length){
        pedsts.append(List(instance_id, i))
      }
      for(i <- 0 until instance.io.out.length){
        pesrcs.append(List(instance_id, i))
      }
    }
    else if(i("type") != "This"){
      val instance_id = i("id").asInstanceOf[Int]
      val module_id = i("module_id").asInstanceOf[Int]
      val cfgBlkIndex = i("cfg_blk_index").asInstanceOf[Int]
      val instance = Module(new hardware(modules(module_id), cfgAddrWidth, cfgDataWidth, cfgBlkIndex, cfgBlkOffset))
      instances(instance_id) = instance
      for(i <- 0 until instance.io.in.length){
        otherdsts.append(List(instance_id, i))
      }
      for(i <- 0 until instance.io.out.length){
        othersrcs.append(List(instance_id, i))
      }
    }
  }

  for((k, v) <- jsonMap("connections").asInstanceOf[Map[String, Any]]){
    val connection = v.asInstanceOf[List[Any]]
    val src = connection(0).asInstanceOf[Int]
    val srctype = connection(1).asInstanceOf[String]
    val srcport = connection(2).asInstanceOf[Int]
    val dst = connection(3).asInstanceOf[Int]
    val dsttype = connection(4).asInstanceOf[String]
    val dstport = connection(5).asInstanceOf[Int]
    val fport = if(src == 0){ io.in(srcport) } else if(srctype == "GPE"){ pes(src).io.out(srcport) } else{ instances(src).io.out(srcport) }
    val tport = if(dst == 0){ io.out(dstport) } else if(dsttype == "GPE"){ pes(dst).io.in(dstport) } else{ instances(dst).io.in(dstport) }
    tport.asInstanceOf[UInt] := fport
    if(srctype == "GPE") { pesrcs -= List(src, srcport) } else if(src > 0) { othersrcs -= List(src, srcport) }
    if(dsttype == "GPE") { pedsts -= List(dst, dstport) } else if(dst > 0) { otherdsts -= List(dst, dstport) }
  }

  for(it <- pesrcs){ pes(it(0)).io.out(it(1)) <> DontCare }
  for(it <- pedsts){ pes(it(0)).io.in(it(1)) := 0.U(dataWidth.W) }
  for(it <- othersrcs){ instances(it(0)).io.out(it(1)) <> DontCare }
  for(it <- otherdsts){ instances(it(0)).io.in(it(1)) := 0.U(dataWidth.W) }
  
  val cfgRegNum = 2 * rows + 4
  val cfgRegs = RegInit(VecInit(Seq.fill(cfgRegNum)(0.U((1 + cfgAddrWidth + cfgDataWidth).W))))
  cfgRegs(0) := Cat(io.cfg_en, io.cfg_addr, io.cfg_data)
  (1 until cfgRegNum).map{ i => cfgRegs(i) := cfgRegs(i-1) }
  for(item <- jsonMap("instances").asInstanceOf[List[Any]]){
    val i = item.asInstanceOf[Map[String, Any]]
    if(i("type") == "GPE"){
      val idx = i("id").asInstanceOf[Int]
      val x = i("x").asInstanceOf[Int]
      val y = i("y").asInstanceOf[Int]
      if(x > 0){
        pes(idx).io.cfg_en := cfgRegs(x - 1)(cfgAddrWidth + cfgDataWidth)
        pes(idx).io.cfg_addr := cfgRegs(x - 1)(cfgAddrWidth + cfgDataWidth - 1, cfgDataWidth)
        pes(idx).io.cfg_data := cfgRegs(x - 1)(cfgDataWidth - 1, 0)
      }
      else{
        pes(idx).io.cfg_en := io.cfg_en
        pes(idx).io.cfg_addr := io.cfg_addr
        pes(idx).io.cfg_data := io.cfg_data
      }
      pes(idx).io.en := io.en((y - 1) / 2)
    }
    else if(i("type") != "This"){
      val idx = i("id").asInstanceOf[Int]
      val x = i("x").asInstanceOf[Int]
      val y = i("y").asInstanceOf[Int]
      if(x > 0){
        instances(idx).io.cfg_en := cfgRegs(x - 1)(cfgAddrWidth + cfgDataWidth)
        instances(idx).io.cfg_addr := cfgRegs(x - 1)(cfgAddrWidth + cfgDataWidth - 1, cfgDataWidth)
        instances(idx).io.cfg_data := cfgRegs(x - 1)(cfgDataWidth - 1, 0)
      }
      else{
        instances(idx).io.cfg_en := io.cfg_en
        instances(idx).io.cfg_addr := io.cfg_addr
        instances(idx).io.cfg_data := io.cfg_data
      }
    }
  }
}

class pe(m: Map[String, Any], cfgAddrWidth: Int, cfgDataWidth: Int, cfgBlkIndex: Int, cfgBlkOffset: Int) extends Module {
  val module_type = m("type").asInstanceOf[String]
  val attributes = m("attributes").asInstanceOf[Map[String, Any]]
  val numIn = attributes("num_input").asInstanceOf[Int]
  val numOut = attributes("num_output").asInstanceOf[Int]
  val dataWidth = attributes("data_width").asInstanceOf[Int]
  var subinstances: mutable.Map[Int, Any] = mutable.Map()
  var wires: mutable.Map[Int, Int] = mutable.Map()
  var numconst = 0
  var sumcfgwidth = 0
  var cfgwidths: mutable.Map[Int, Tuple2[String, Int]] = mutable.Map()
  val configurations = if(attributes.contains("configuration")) { attributes("configuration").asInstanceOf[Map[String, List[Any]]] } else { Map[String, List[Any]]() }
  var srcs = new ListBuffer[Int]
  var dsts = new ListBuffer[Int]
  for(i <- 0 until numIn) { srcs.append(i) }
  for(o <- 0 until numOut) { dsts.append(o) }

  val io = IO(new Bundle{
    val cfg_en   = Input(Bool())
    val cfg_addr = Input(UInt(cfgAddrWidth.W))
    val cfg_data = Input(UInt(cfgDataWidth.W))
    val en  = Input(Bool()) // top and buttom row
    val in  = Input(Vec(numIn, UInt(dataWidth.W)))
    val out = Output(Vec(numOut, UInt(dataWidth.W)))
  })

  for(i <- attributes("instances").asInstanceOf[List[Any]]){
    val subinstance = i.asInstanceOf[Map[String, Any]]
    if(subinstance("type") == "Muxn"){
      val subinstance_id = subinstance("id").asInstanceOf[Int]
      var innum = 0
      for((k, v) <- attributes("connections").asInstanceOf[Map[String, Any]]){
        val connection = v.asInstanceOf[List[Any]]
        val dstid = connection(3).asInstanceOf[Int]
        if(dstid == subinstance_id){
          innum += 1
        }
      }
      val muxcfgwidth = log2Ceil(innum)
      val mux = Module(new Muxn(dataWidth, innum))
      subinstances(subinstance_id) = mux
      sumcfgwidth += muxcfgwidth
      cfgwidths(subinstance_id) = Tuple2(subinstance("type").asInstanceOf[String], muxcfgwidth)
    }
  }
    val operations = attributes("operations").asInstanceOf[List[String]]
    val operandnum = operations.map(OPC.withName(_)).map(OpInfo.getOperandNum(_)).max
    var ops: ListBuffer[OPC.OPC] = new ListBuffer[OPC.OPC]
    ops ++= operations.map(OPC.withName(_))
    val max_delay = attributes("max_delay").asInstanceOf[Int]
    for(i <- attributes("instances").asInstanceOf[List[Any]]){
      val subinstance = i.asInstanceOf[Map[String, Any]]
      val subinstance_id = subinstance("id").asInstanceOf[Int]
      if(subinstance("type") == "ALU"){
        val alu = Module(new ALU(dataWidth, ops))
        subinstances(subinstance_id) = alu
        val alucfgwidth = log2Ceil(OPC.numOPC)
        sumcfgwidth += alucfgwidth
        cfgwidths(subinstance_id) = Tuple2(subinstance("type").asInstanceOf[String], alucfgwidth)
      }
      else if(subinstance("type") == "RF"){
        var innum = 0
        var outnum = 0
        var regnum = 1
        for((k, v) <- attributes("connections").asInstanceOf[Map[String, Any]]){
          val connection = v.asInstanceOf[List[Any]]
          val srcid = connection(0).asInstanceOf[Int]
          val srcportid = connection(2).asInstanceOf[Int]
          val dstid = connection(3).asInstanceOf[Int]
          val dstportid = connection(5).asInstanceOf[Int]
          if(srcid == subinstance_id && srcportid >= outnum){
            outnum = srcportid + 1
          }
          if(dstid == subinstance_id && dstportid >= innum){
            innum = dstportid + 1
          }
        }
        if(attributes.contains("configuration")){
        for((k, v) <- configurations){
          if(k.toInt == subinstance_id){
            val rfconfigwidth = v(1).asInstanceOf[Int] - v(2).asInstanceOf[Int] + 1
            val rfselwidth = rfconfigwidth / (innum + outnum)
            println("rfselwidth: " + rfselwidth)
            regnum = pow(2, rfselwidth).toInt
          }
        }
        }
        val rf = Module(new RF(dataWidth, regnum, innum, outnum))
        subinstances(subinstance_id) = rf
        rf.io.en := io.en
        if(rf.io.config.getWidth == 0) { rf.io.config := DontCare }
      }
      else if(subinstance("type") == "DelayPipe"){
        val delay_pipe = Module(new SharedDelayPipe(dataWidth, max_delay, operandnum))
        subinstances(subinstance_id) = delay_pipe
        delay_pipe.io.en := io.en
        val delaycfgwidth = operandnum * log2Ceil(max_delay + 1)
        sumcfgwidth += delaycfgwidth
        cfgwidths(subinstance_id) = Tuple2(subinstance("type").asInstanceOf[String], delaycfgwidth)
      }
      else if(subinstance("type") == "Const"){        
        wires(subinstance_id) = numconst
        numconst += 1
        val constcfgwidth = dataWidth
        sumcfgwidth += constcfgwidth
        cfgwidths(subinstance_id) = Tuple2(subinstance("type").asInstanceOf[String], constcfgwidth)
      }
    }

  val consts = Wire(Vec(numconst, UInt(dataWidth.W)))
  if(numconst == 0) { consts <> DontCare }

  for((k, v) <- attributes("connections").asInstanceOf[Map[String, Any]]){
    val connection = v.asInstanceOf[List[Any]]
    val src = connection(0).asInstanceOf[Int]
    val srctype = connection(1).asInstanceOf[String]
    val srcport = connection(2).asInstanceOf[Int]
    val dst = connection(3).asInstanceOf[Int]
    val dsttype = connection(4).asInstanceOf[String]
    val dstport = connection(5).asInstanceOf[Int]
    val fport = srctype match {
      case "Const" => consts(wires(src))
      case "Muxn" => subinstances(src).asInstanceOf[Muxn].io.out(srcport)
      case "ALU" => subinstances(src).asInstanceOf[ALU].io.out(srcport)
      case "DelayPipe" => subinstances(src).asInstanceOf[SharedDelayPipe].io.out(srcport)
      case "RF" => subinstances(src).asInstanceOf[RF].io.out(srcport)
      case _ => io.in(srcport)
    }
    val tport = dsttype match {
      case "Muxn" => subinstances(dst).asInstanceOf[Muxn].io.in(dstport)
      case "ALU" => subinstances(dst).asInstanceOf[ALU].io.in(dstport)
      case "DelayPipe" => subinstances(dst).asInstanceOf[SharedDelayPipe].io.in(dstport)
      case "RF" => subinstances(dst).asInstanceOf[RF].io.in(dstport)
      case _ => io.out(dstport)
    }
    tport.asInstanceOf[UInt] := fport
    if(src == 0) { srcs -= srcport }
    if(dst == 0) { dsts -= dstport }
  }

  for(it <- srcs) { io.in(it) <> DontCare }
  for(it <- dsts) { io.out(it) := 0.U(io.out(it).getWidth.W) }

  if(attributes.contains("configuration")){
  sumcfgwidth = configurations("0")(1).asInstanceOf[Int] + 1
  val cfg = Module(new ConfigMem(sumcfgwidth, 1, cfgDataWidth))
  cfg.io.cfg_en := io.cfg_en && (cfgBlkIndex.U === io.cfg_addr(cfgAddrWidth-1, cfgBlkOffset))
  cfg.io.cfg_addr := io.cfg_addr(cfgBlkOffset-1, 0)
  cfg.io.cfg_data := io.cfg_data
  val cfgout = Wire(UInt(sumcfgwidth.W))
  cfgout := cfg.io.out(0)
  for((k, v) <- configurations){
    val cid = k.toInt
    val ctype = v(0).asInstanceOf[String]
    if(cid > 0){
      val high = v(1).asInstanceOf[Int]
      val low = v(1).asInstanceOf[Int]
      val to = ctype match {
        case "Const" => consts(wires(cid))
        case "Muxn" => subinstances(cid).asInstanceOf[Muxn].io.config
        case "ALU" => subinstances(cid).asInstanceOf[ALU].io.config
        case "DelayPipe" => subinstances(cid).asInstanceOf[SharedDelayPipe].io.config
        case "RF" => subinstances(cid).asInstanceOf[RF].io.config
      }
      to := cfgout(high, low)
    }
  }
  }
  else if(sumcfgwidth > 0){
    val cfg = Module(new ConfigMem(sumcfgwidth, 1, cfgDataWidth))
    cfg.io.cfg_en := io.cfg_en && (cfgBlkIndex.U === io.cfg_addr(cfgAddrWidth-1, cfgBlkOffset))
    cfg.io.cfg_addr := io.cfg_addr(cfgBlkOffset-1, 0)
    cfg.io.cfg_data := io.cfg_data
    val cfgout = Wire(UInt(sumcfgwidth.W))
    cfgout := cfg.io.out(0)
    var low = 0
    var high = 0
    for((k, v) <- cfgwidths){
      high = low + v._2 - 1
      val to = v._1 match {
        case "Const" => consts(wires(k))
        case "Muxn" => subinstances(k).asInstanceOf[Muxn].io.config
        case "ALU" => subinstances(k).asInstanceOf[ALU].io.config
        case "DelayPipe" => subinstances(k).asInstanceOf[SharedDelayPipe].io.config
        case "RF" => subinstances(k).asInstanceOf[RF].io.config
      }
      to := cfgout(high, low)
      low = high + 1
    }
  }
  else{
    io.cfg_en <> DontCare
    io.cfg_addr <> DontCare
    io.cfg_data <> DontCare
  }
}

class hardware(m: Map[String, Any], cfgAddrWidth: Int, cfgDataWidth: Int, cfgBlkIndex: Int, cfgBlkOffset: Int) extends Module {
  val module_type = m("type").asInstanceOf[String]
  val attributes = m("attributes").asInstanceOf[Map[String, Any]]
  val numIn = attributes("num_input").asInstanceOf[Int]
  val numOut = attributes("num_output").asInstanceOf[Int]
  val dataWidth = attributes("data_width").asInstanceOf[Int]
  var subinstances: mutable.Map[Int, Any] = mutable.Map()
  var wires: mutable.Map[Int, Int] = mutable.Map()
  var numconst = 0
  var sumcfgwidth = 0
  var cfgwidths: mutable.Map[Int, Tuple2[String, Int]] = mutable.Map()
  val configurations = if(attributes.contains("configuration")) { attributes("configuration").asInstanceOf[Map[String, List[Any]]] } else { Map[String, List[Any]]() }
  var srcs = new ListBuffer[Int]
  var dsts = new ListBuffer[Int]
  for(i <- 0 until numIn) { srcs.append(i) }
  for(o <- 0 until numOut) { dsts.append(o) }
  //var muxnum = 0

  val io = IO(new Bundle{
    val cfg_en   = Input(Bool())
    val cfg_addr = Input(UInt(cfgAddrWidth.W))
    val cfg_data = Input(UInt(cfgDataWidth.W))
    val in  = Input(Vec(numIn, UInt(dataWidth.W)))
    val out = Output(Vec(numOut, UInt(dataWidth.W)))
  })

  for(i <- attributes("instances").asInstanceOf[List[Any]]){
    val subinstance = i.asInstanceOf[Map[String, Any]]
    if(subinstance("type") == "Muxn"){
      val subinstance_id = subinstance("id").asInstanceOf[Int]
      var innum = 0
      for((k, v) <- attributes("connections").asInstanceOf[Map[String, Any]]){
        val connection = v.asInstanceOf[List[Any]]
        val dstid = connection(3).asInstanceOf[Int]
        if(dstid == subinstance_id){
          innum += 1
        }
      }
      val muxcfgwidth = log2Ceil(innum)
      val mux = Module(new Muxn(dataWidth, innum))
      subinstances(subinstance_id) = mux
      sumcfgwidth += muxcfgwidth
      cfgwidths(subinstance_id) = Tuple2(subinstance("type").asInstanceOf[String], muxcfgwidth)
    }
  }

  val consts = Wire(Vec(numconst, UInt(dataWidth.W)))
  if(numconst == 0) { consts <> DontCare }

  for((k, v) <- attributes("connections").asInstanceOf[Map[String, Any]]){
    val connection = v.asInstanceOf[List[Any]]
    val src = connection(0).asInstanceOf[Int]
    val srctype = connection(1).asInstanceOf[String]
    val srcport = connection(2).asInstanceOf[Int]
    val dst = connection(3).asInstanceOf[Int]
    val dsttype = connection(4).asInstanceOf[String]
    val dstport = connection(5).asInstanceOf[Int]
    val fport = srctype match {
      case "Const" => consts(wires(src))
      case "Muxn" => subinstances(src).asInstanceOf[Muxn].io.out(srcport)
      case "ALU" => subinstances(src).asInstanceOf[ALU].io.out(srcport)
      case "DelayPipe" => subinstances(src).asInstanceOf[SharedDelayPipe].io.out(srcport)
      case "RF" => subinstances(src).asInstanceOf[RF].io.out(srcport)
      case _ => io.in(srcport)
    }
    val tport = dsttype match {
      case "Muxn" => subinstances(dst).asInstanceOf[Muxn].io.in(dstport)
      case "ALU" => subinstances(dst).asInstanceOf[ALU].io.in(dstport)
      case "DelayPipe" => subinstances(dst).asInstanceOf[SharedDelayPipe].io.in(dstport)
      case "RF" => subinstances(dst).asInstanceOf[RF].io.in(dstport)
      case _ => io.out(dstport)
    }
    if(module_type == "GIB" && srctype == "Muxn"){ tport.asInstanceOf[UInt] := RegNext(fport) }
    else{ tport.asInstanceOf[UInt] := fport }
    if(src == 0) { srcs -= srcport }
    if(dst == 0) { dsts -= dstport }
  }

  for(it <- srcs) { io.in(it) <> DontCare }
  for(it <- dsts) { io.out(it) := 0.U(io.out(it).getWidth.W) }

  if(attributes.contains("configuration")){
  sumcfgwidth = configurations("0")(1).asInstanceOf[Int] + 1
  val cfg = Module(new ConfigMem(sumcfgwidth, 1, cfgDataWidth))
  cfg.io.cfg_en := io.cfg_en && (cfgBlkIndex.U === io.cfg_addr(cfgAddrWidth-1, cfgBlkOffset))
  cfg.io.cfg_addr := io.cfg_addr(cfgBlkOffset-1, 0)
  cfg.io.cfg_data := io.cfg_data
  val cfgout = Wire(UInt(sumcfgwidth.W))
  cfgout := cfg.io.out(0)
  for((k, v) <- configurations){
    val cid = k.toInt
    val ctype = v(0).asInstanceOf[String]
    if(cid > 0){
      val high = v(1).asInstanceOf[Int]
      val low = v(1).asInstanceOf[Int]
      val to = ctype match {
        case "Const" => consts(wires(cid))
        case "Muxn" => subinstances(cid).asInstanceOf[Muxn].io.config
        case "ALU" => subinstances(cid).asInstanceOf[ALU].io.config
        case "DelayPipe" => subinstances(cid).asInstanceOf[SharedDelayPipe].io.config
        case "RF" => subinstances(cid).asInstanceOf[RF].io.config
      }
      to := cfgout(high, low)
    }
  }
  }
  else if(sumcfgwidth > 0){
    val cfg = Module(new ConfigMem(sumcfgwidth, 1, cfgDataWidth))
    cfg.io.cfg_en := io.cfg_en && (cfgBlkIndex.U === io.cfg_addr(cfgAddrWidth-1, cfgBlkOffset))
    cfg.io.cfg_addr := io.cfg_addr(cfgBlkOffset-1, 0)
    cfg.io.cfg_data := io.cfg_data
    val cfgout = Wire(UInt(sumcfgwidth.W))
    cfgout := cfg.io.out(0)
    var low = 0
    var high = 0
    for((k, v) <- cfgwidths){
      high = low + v._2 - 1
      val to = v._1 match {
        case "Const" => consts(wires(k))
        case "Muxn" => subinstances(k).asInstanceOf[Muxn].io.config
        case "ALU" => subinstances(k).asInstanceOf[ALU].io.config
        case "DelayPipe" => subinstances(k).asInstanceOf[SharedDelayPipe].io.config
        case "RF" => subinstances(k).asInstanceOf[RF].io.config
      }
      to := cfgout(high, low)
      low = high + 1
    }
  }
  else{
    io.cfg_en <> DontCare
    io.cfg_addr <> DontCare
    io.cfg_data <> DontCare
  }
}


class arch_info(filename: String, getArea: Boolean){
  val jsonMap = IRHandler.loadIR(filename)
  if(getArea){
    val area = estimator.get_area(jsonMap)
    println("CGRA area: " + area.formatted("%.2f"))
  }
  def get_attrs() = jsonMap
}



