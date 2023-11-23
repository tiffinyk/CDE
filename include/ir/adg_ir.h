#ifndef __ADG_IR_H__
#define __ADG_IR_H__

#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include "nlohmann/json.hpp"
#include "adg/adg.h"

using json = nlohmann::json; 


class ADGIR
{
private:
    ADG* _adg;
    // parse ADG json object
    ADG* parseADG(json& adgJson);
    // parse ADGNode from sub-modules json object 
    ADGNode* parseADGNode(json& nodeJson);
    // parse ADGNode from instances json object, 
    // modules<moduleId, <ADGNode*, used>>,  
    ADGNode* parseADGNode(json& nodeJson, std::map<int, std::pair<ADGNode*, bool>>& modules);
    // parse ADGLink json object
    void parseADGLinks(ADG* adg, json& linkJson);
    // analyze the connections among the internal sub-modules for GPENode
    // fill _operandInputs 
    void analyzeIntraConnect(ADGNode* node);
    // analyze if there are registers in the output ports of GIB
    // fill _outReged
    void analyzeOutReg(ADG* adg, ADGNode* node);
    // post-process the ADG nodes
    void postProcess(ADG* adg);
public:
    ADGIR(std::string filename);
    ADGIR(ADG*& adg) { _adg = adg; }
    ~ADGIR();
    ADG* getADG(){ return _adg; }
    void setADG(ADG*& adg) { _adg = adg; }
    void saveADG(std::ofstream& );
};





#endif