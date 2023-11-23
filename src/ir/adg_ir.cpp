
#include "ir/adg_ir.h"


ADGIR::ADGIR(std::string filename)
{
    std::ifstream ifs(filename);
    if(!ifs){
        std::cout << "Cannnot open ADG file: " << filename << std::endl;
        exit(1);
    }
    json adgJson;
    ifs >> adgJson;
    _adg = parseADG(adgJson);
}

ADGIR::~ADGIR()
{
    /*if(_adg){
        delete _adg;
    }*/
}


// parse ADG json object
ADG* ADGIR::parseADG(json& adgJson){
    // std::cout << "Parse ADG..." << std::endl;
    ADG* adg = new ADG();
    adg->setBitWidth(adgJson["data_width"].get<int>());
    adg->setNumInputs(adgJson["num_input"].get<int>());
    adg->setNumOutputs(adgJson["num_output"].get<int>());  
    if(adgJson.contains("cfg_data_width")){
        adg->setCfgDataWidth(adgJson["cfg_data_width"].get<int>());
        adg->setCfgAddrWidth(adgJson["cfg_addr_width"].get<int>()); // they are together
        adg->setCfgBlkOffset(adgJson["cfg_blk_offset"].get<int>());
    }
    std::map<int, std::pair<ADGNode*, bool>> modules; // // <moduleId, <ADGNode*, used>>
    //std::cout << "Parse submodules start." << std::endl;
    for(auto& nodeJson : adgJson["sub_modules"]){
        ADGNode* node = parseADGNode(nodeJson);
        modules[node->id()] = std::make_pair(node, false);
    }
    //std::cout << "Parse submodules succeed." << std::endl;
    for(auto& nodeJson : adgJson["instances"]){
        ADGNode* node = parseADGNode(nodeJson, modules);
        int nodeId = nodeJson["id"].get<int>();
        if(node){ // not store sub-module of "This" type
            adg->addNode(nodeId, node);
        }else{ // "This" sub-module
            adg->setId(nodeId);
        }
    }
    //std::cout << "Parse instances succeed." << std::endl;
    parseADGLinks(adg, adgJson["connections"]);
    //std::cout << "Parse connections succeed." << std::endl;
    postProcess(adg);  
    //std::cout << "Post process succeed." << std::endl;
    return adg; 
}


// parse ADGNode from sub-modules json object 
ADGNode* ADGIR::parseADGNode(json& nodeJson){
    // std::cout << "Parse ADG node" << std::endl;
    std::string type = nodeJson["type"].get<std::string>();
    // if(type == "This"){
    //     return nullptr;
    // }
    int nodeId = nodeJson["id"].get<int>();
    ADGNode* adg_node = new ADGNode(nodeId);
    if(type == "GPE" || type == "GIB" || type == "IB" || type == "OB"){
        auto& attrs = nodeJson["attributes"];
        int bitWidth = attrs["data_width"].get<int>();
        int numInputs = attrs["num_input"].get<int>();
        int numOutputs = attrs["num_output"].get<int>();
        int cfgBlkIdx = attrs["cfg_blk_index"].get<int>();
        // std::cout << "Parse sub-ADG..." << std::endl;   
        if(type == "GPE"){
            adg_node->setNumRfReg(attrs["num_rf_reg"].get<int>());
            // node->setMaxDelay(attrs["max_delay"].get<int>());
            adg_node->setNumOperands(attrs["num_operands"].get<int>());
            for(auto& op : attrs["operations"]){
                adg_node->addOperation(op.get<std::string>());
            }
        }
        //std::cout << "Parse sub ADG of " << type << nodeId << std::endl;
        ADG* subADG = parseADG(attrs); // parse sub-adg
        //std::cout << "Parse sub ADG succeed." << std::endl;
        adg_node->setType(type);
        adg_node->setBitWidth(bitWidth);
        adg_node->setNumInputs(numInputs);
        adg_node->setNumOutputs(numOutputs);
        adg_node->setCfgBlkIdx(cfgBlkIdx);
        adg_node->setSubADG(subADG);
        if(attrs.count("configuration")){
            for(auto& elem : attrs["configuration"].items()){
                int subModuleId = std::stoi(elem.key());
                auto& info = elem.value();
                CfgDataLoc cfg;
                cfg.high = info[1].get<int>();
                cfg.low = info[2].get<int>();
                adg_node->addConfigInfo(subModuleId, cfg);
            }
        }
    }else{ // common components: ALU, Muxn, RF, DelayPipe, Const
        adg_node = new ADGNode(nodeId);
        adg_node->setType(type);
    } 
    return adg_node;
}


// parse ADGNode from instances json object, 
// modules<moduleId, <ADGNode*, used>>,  
ADGNode* ADGIR::parseADGNode(json& nodeJson, std::map<int, std::pair<ADGNode*, bool>>& modules){
    std::string type = nodeJson["type"].get<std::string>();
    if(type == "This"){
        return nullptr;
    }
    int nodeId = nodeJson["id"].get<int>();
    int moduleId = nodeJson["module_id"].get<int>();
    ADGNode* adg_node;
    ADGNode* module = modules[moduleId].first;
    bool renewNode = modules[moduleId].second; // used, need to re-new ADGNode
    if(type == "GPE" || type == "GIB" || type == "IB" || type == "OB"){                
        if(renewNode){ // re-new ADGNode
            adg_node = new ADGNode(nodeId);
            *adg_node = *module;
            ADG* subADG = new ADG(); 
            *subADG = *(adg_node->subADG()); // COPY Sub-ADG
            adg_node->setSubADG(subADG);
        }else{ // reuse the ADGNode in modules
            adg_node = module;
            modules[moduleId].second = true;
        }
        if(type == "GIB"){
            bool trackReged = nodeJson["track_reged"].get<bool>(); 
            adg_node->setTrackReged(trackReged);
        }else if(type == "GPE"){
            adg_node->setMaxDelay(nodeJson["max_delay"].get<int>());
        }
        int cfgBlkIdx = nodeJson["cfg_blk_index"].get<int>();    
        int x = nodeJson["x"].get<int>(); 
        int y = nodeJson["y"].get<int>(); 
        adg_node->setCfgBlkIdx(cfgBlkIdx);  
        adg_node->setX(x);     
        adg_node->setY(y);          
    }else{ // common components: ALU, Muxn, RF, DelayPipe, Const
        if(renewNode){ // re-new ADGNode
            adg_node = new ADGNode(nodeId);
            *adg_node = *module;  
        }else{ // reuse the ADGNode in modules
            adg_node = module;
            modules[moduleId].second = true;
        }      
    } 
    adg_node->setId(nodeId);
    adg_node->setType(type);
    return adg_node;
}


// parse ADGLink json object
void ADGIR::parseADGLinks(ADG* adg, json& linkJson){
    // std::cout << "Parse ADG Link" << std::endl;
    for(auto& elem : linkJson.items()){
        int linkId = std::stoi(elem.key());
        auto& link = elem.value();
        int srcId = link[0].get<int>();
        // std::string srcType = link[1].get<std::string>();
        int srcPort = link[2].get<int>();
        int dstId = link[3].get<int>();
        // std::string dstType = link[4].get<std::string>();
        int dstPort = link[5].get<int>();
        ADGLink* adg_link = new ADGLink(srcId, dstId);
        adg_link->setId(linkId);
        adg_link->setSrcId(srcId);
        adg_link->setDstId(dstId);
        adg_link->setSrcPortIdx(srcPort);
        adg_link->setDstPortIdx(dstPort);
        adg->addLink(linkId, adg_link);
    }
}


// analyze the connections among the internal sub-modules for GPENode
// fill _operandInputs 
void ADGIR::analyzeIntraConnect(ADGNode* node){
    ADG* subAdg = node->subADG();
    for(auto& elem : subAdg->inputs()){
        if(node->type() == "GPE"){
            auto input = elem.second.begin(); // one input only connected to one sub-module
            ADGNode* subNode = subAdg->node(input->first);
            //std::cout<< "subnode: " << input->first << " found." << std::endl;
            int opeIdx = input->second; // operand index
            //std::cout<< "operand: " << input->second << " found." << std::endl;
            while (subNode->type() != "ALU"){
                if(subNode->outputs().size() == 1){ // only one output
                    opeIdx = 0;
                }
                auto out = subNode->output(opeIdx).begin(); 
                subNode = subAdg->node(out->first);
                opeIdx = out->second;
            }
            // opeIdx is ALU operand index now
            node->addOperandInputs(opeIdx, elem.first);
        }
        else{
            int inPort = elem.first;
            for(auto& subNode : elem.second){
                int outPort;
                if(subNode.first == subAdg->id()){ // input directly connected to output
                    outPort = subNode.second;
                } else {
                    ADGNode* subNodePtr = subAdg->node(subNode.first);
                    outPort = subNodePtr->output(0).begin()->second; // only one layer of Muxn
                }
                node->addIn2outs(inPort, outPort);
                node->addOut2ins(outPort, inPort);
            }
        }
    }
}


// analyze if there are registers in the output ports of GIB
// fill _outReged
void ADGIR::analyzeOutReg(ADG* adg, ADGNode* node){
    for(auto& elem : node->outputs()){
        int id = elem.second.begin()->first; // GIB output port only connected to one node
        if(node->trackReged() && (adg->node(id)->type() == "GIB")){ // this edge is track
            node->setOutReged(elem.first, true);
        }else{
            node->setOutReged(elem.first, false);
        }
    }
}


// post-process the ADG nodes
void ADGIR::postProcess(ADG* adg){
    int numGpeNodes = 0;
    for(auto& node : adg->nodes()){
        auto nodePtr = node.second;
        if(nodePtr->type() == "GPE"){
            //std::cout << "GPE." << std::endl;
            numGpeNodes++;
            analyzeIntraConnect(nodePtr);
        } else if(nodePtr->type() == "GIB"){
            analyzeIntraConnect(nodePtr);
            analyzeOutReg(adg, nodePtr);
            //std::cout << "GIB." << std::endl;
        } else if(nodePtr->type() == "IB" || nodePtr->type() == "OB"){
            analyzeIntraConnect(nodePtr);
            //std::cout << "IOB." << std::endl;
        }
    }
    adg->setNumGpeNodes(numGpeNodes);
}

void ADGIR::saveADG(std::ofstream& os)
{
    json adgJson, modules, instances, connections;
    int num_col = (_adg->numInputs()) >> 1; //It is different from rows and cols in generator.
    int num_row = (_adg->numGpeNodes()) / num_col;
    adgJson["num_row"] = num_row;
    adgJson["num_colum"] = num_col;
    adgJson["data_width"] = _adg->bitWidth();
    adgJson["num_input"] = _adg->numInputs();
    adgJson["num_output"] = _adg->numOutputs();
    adgJson["cfg_data_width"] = _adg->cfgDataWidth();
    adgJson["cfg_addr_width"] = _adg->cfgAddrWidth();
    adgJson["cfg_blk_offset"] = _adg->cfgBlkOffset();
    //parse ADG nodes
    for(const auto& it : _adg->nodes()){
        ADGNode* node = it.second;
        std::string type = node->type();
        json instance, cgramodule, attributes;
        //set instance parameters
        instance["id"] = node->id();
        instance["module_id"] = node->id();
        instance["cfg_blk_index"] = node->cfgBlkIdx();
        instance["x"] = node->x();
        instance["y"] = node->y();
        instance["type"] = type;
        if(type == "GIB"){
            instance["track_reged"] = node->trackReged();
        }else if(type == "GPE"){
            instance["max_delay"] = node->maxDelay();
        }
        instances.emplace_back(instance);
        //set module parameters
        cgramodule["id"] = it.first;
        cgramodule["type"] = type;
        attributes["data_width"] = node->bitWidth();
        attributes["num_input"] = node->numInputs();
        attributes["num_output"] = node->numOutputs();
        attributes["cfg_blk_index"] = node->cfgBlkIdx();
        //add the top instance
        json thisinstance;
        thisinstance["id"] = 0;
        thisinstance["type"] = "This";
        thisinstance["module_id"] = 0;
        attributes["instances"].emplace_back(thisinstance);
        if(type == "GIB"){
            attributes["track_reged"] = node->trackReged();
        }
        else if(type == "GPE"){
            attributes["num_rf_reg"] = node->numRfReg();
            attributes["num_operands"] = node->numOperands();
            attributes["max_delay"] = node->maxDelay();
            for(const auto& op : node->operations()){
                attributes["operations"].emplace_back(op);
            }
        }
        // parse sub ADG
        json submodule, subinstance, subconnections;
        ADG* subADG = node->subADG();
        ADGNode* subnode;
        ADGLink* sublink;
        for (const auto& it : subADG->nodes()) {
            subnode = it.second;
            submodule["type"] = subnode->type();
            submodule["id"] = subnode->id();
            attributes["sub_modules"].emplace_back(submodule);
            subinstance["type"] = subnode->type();
            subinstance["id"] = subnode->id();
            subinstance["module_id"] = subnode->id();
            attributes["instances"].emplace_back(subinstance);
        }
        for(const auto& it : subADG->links()){
            sublink = it.second;
            std::string sublinkid = std::to_string(it.first);
            int srcId = sublink->srcId();
            std::string srcType = srcId > 0 ? subADG->node(srcId)->type() : "This";
            int srcPortIdx = sublink->srcPortIdx();
            int dstId = sublink->dstId();
            std::string dstType = dstId > 0 ? subADG->node(dstId)->type() : "This";
            int dstPortIdx = sublink->dstPortIdx();
            subconnections[sublinkid] = { srcId, srcType, srcPortIdx, dstId, dstType, dstPortIdx };
        }
        attributes["connections"] = subconnections;
        
        /*for(int i = 0; i < node->numInputs(); i++){
            std::string linkid = std::to_string(i);
            subconnections[linkid] = {0, "This", i, 0, "This", 0};
        }*/
        cgramodule["attributes"] = attributes;
        modules.emplace_back(cgramodule);
    }
    //add top instance
    json topinstance;
    topinstance["id"] = _adg->id();
    topinstance["type"] = "This";
    instances.emplace_back(topinstance);
    //parse ADG links
    for(const auto& it : _adg->links()){
        ADGLink* link = it.second;
        std::string id = std::to_string(it.first);
        int srcId = link->srcId();
        std::string srcType = srcId > 0 ? _adg->node(srcId)->type() : "This";
        int srcPortIdx = link->srcPortIdx();
        int dstId = link->dstId();
        std::string dstType = dstId > 0 ? _adg->node(dstId)->type() : "This";
        int dstPortIdx = link->dstPortIdx();
        connections[id] = {srcId, srcType, srcPortIdx, dstId, dstType, dstPortIdx};
    }
    adgJson["sub_modules"] = modules;
    adgJson["instances"] = instances;
    adgJson["connections"] = connections;
    os << adgJson.dump(2) << std::endl;
}