package ppa

object area_par {
  val area_mux32_map :Map[Int, Double] = Map(
    1->0.0,
    2->4.39,
    3->5.95,
    4->8.05,
    5->10.22,
    6->12.20,
    7->13.73,
    8->15.56,
    9->19.30,
    10->20.92,
    11->22.92,
    12->23.18,
    13->24.68,
    14->28.49,
    15->29.58,
    16->34.19,
    17->35.34,
    18->37.35,
    19->38.15,
    20->39.10,
    21->42.81,
    22->43.04,
    23->47.47,
    24->48.71,
  )
  val area_mux32_per_in = 2.03 // innum > 24 => estimation

  val area_delay32_map : Map[Int, Map[Int,Double]] = Map(
    1 -> Map(
      1 -> 34.16,
      2 -> 48.00,
      3 -> 61.98,
      4 -> 77.65,
      5 -> 94.30,
      6 -> 107.6,
      7 -> 122.49
    ),
    2 -> Map(
      1 -> 62.27,
      2 -> 82.36,
      3 -> 102.40,
      4 -> 125.45,
      5 -> 145.26,
      6 -> 160.64,
      7 -> 184.39
    ),
    3 -> Map(
      1 -> 98.95,
      2 -> 130.45,
      3 -> 150.51,
      4 -> 176.11,
      5 -> 197.65,
      6 -> 230.61,
      7 -> 250.53
    ),
  )
  val area_delay32_per_cycle = 17.5 // cycle > 7 => estimation

  val area_alu32_map : Map[String, Double] = Map(
    "PASS" -> 0.00,
    "ADD" -> 9.26,
    "SUB" -> 10.66,
    "MUL" -> 174.61,
    "AND" -> 2.80,
    "OR" -> 2.80,
    "XOR" -> 4.20,
    "SHL" -> 15.79,
    "LSHR" -> 15.83,
    "ASHR" -> 16.53,
    "EQ" -> 5.28,//
    "NE" -> 5.28,
    "LT" -> 6.04,//
    "LE" -> 5.99,
    "SEL" -> 4.24,
    "group1" -> 10.66,
    "group2" -> 4.20,
    "group3" -> 15.83,
    "group4" -> 174.61,
    "group5" -> 6.04,
    "group6" -> 0.00
  )
  val op_group : Map [List[String] ,String] = Map(
    ( List("ADD","SUB" )->"group1"),
    ( List("AND","OR","XOR" )->"group2"),
    ( List("SHL","LSHR","ASHR" )->"group3"),
    ( List("MUL" )->"group4"),
    ( List("EQ","NE","LT","LE" )->"group5"),
    ( List("PASS" )->"group6")
  );
  val area_regnxt32 = 8.00
  val area_rf32 = 12.29
  val area_rf8 = 3.19

  val area_cfgpre32 = 0.40 //0.10

  val area_const32 = 0

  val cycle = 5
  val reduce_rate = 0.3

}

