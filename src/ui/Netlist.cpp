#include "ui/Netlist.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

Netlist::Netlist(Mapping* mapping)
{
    ADG* adg = mapping->getADG();
    DFG* dfg = mapping->getDFG();

    for(auto& elem : dfg->nodes()){
        auto dfgNode = elem.second;
        auto adgNode = mapping->mappedNode(dfgNode);

        _nodes[adgNode->id()] = std::make_pair(elem.first, dfgNode->name());
    }

    for(auto& elem : dfg->edges()){
        int eid = elem.first;
        auto& attr = mapping->dfgEdgeAttr(eid);
        auto& edgeLinks = attr.edgeLinks;
        int i = 0;
        int linkId;
        for(auto& edgeLink : edgeLinks){
            auto adgNode = edgeLink.adgNode;
            int dstId = adgNode->id();
            if(_nodes.count(dstId) == 0){
                _switches.emplace(dstId);
            }
            if(i > 0){
                /*if(adgNode->type() == "IB"){
                    int inport = adgNode->input(edgeLink.srcPort).second; // IB connected to ADG Input port
                }*/
                //linkId = adg->net(srcId, edgeLink.dstPort)->id();
                int dstPortIdx = edgeLink.srcPort;
                for(auto& link : adg->links()){
                    if(dstId == link.second->dstId() && dstPortIdx == link.second->dstPortIdx()) { linkId = link.second->id(); }
                }
                //std::cout << "dstId: " << dstId << ", dstPortIdx: " << dstPortIdx << "linkId: " << linkId << std::endl;
                _nets.emplace(linkId);
            }
            i++;
            /*else{
                if(adgNode->type() == "OB"){
                    int outport = adgNode->output(edgeLink.dstPort).begin()->second; // OB connected to ADG output port
                }
            }*/
        }
    }
}

void Netlist::load(std::string filename)
{
    std::ifstream ifs(filename);
    if(!ifs){
        std::cout << "Cannnot open Netlist file: " << filename << std::endl;
        exit(1);
    }
    json netlistJson;
    ifs >> netlistJson;
    for(auto& netJson : netlistJson["nets"])
    {
        _nets.emplace(netJson["adg_link"].get<int>());
    }
    for(auto& nodeJson : netlistJson["nodes"])
    {
        if(nodeJson.contains("dfg_node"))
        {
            _nodes[nodeJson["adg_node"].get<int>()] = std::make_pair(nodeJson["dfg_node"].get<int>(), nodeJson["instruction"].get<std::string>());
        }
        else
        {
            _switches.emplace(nodeJson["adg_node"].get<int>());
        }
    }
}

void Netlist::saveNetlist(std::ostream &os, ADG* adg)
{
	json netlist, nodes, nets, node, net;

    for (auto& elem : _switches)
    {
        node["adg_node"] = elem;
        node["type"] = adg->node(elem)->type();
        nodes.emplace_back(node);
    }

    for(auto& elem : _nodes)
    {
        node["adg_node"] = elem.first;
        node["dfg_node"] = elem.second.first;
        node["instruction"] = elem.second.second;
        node["type"] = adg->node(elem.first)->type();
        nodes.emplace_back(node);
    }

    for(int linkId : _nets)
    {
        net["adg_link"] = linkId;
        nets.emplace_back(net);
    }
    netlist["nodes"] = nodes;
    netlist["nets"] = nets;
    os << netlist << std::endl;
}
