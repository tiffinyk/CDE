#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include "mapper/mapping.h"
// #include "mapper/candidate.h"
#include "mapper/visualize.h"
#include "mapper/configuration.h"
#include "spdlog/spdlog.h"


// CGRA ADG Generator: Generate ADG according to DFGs
class Generator
{
private:
    int X;
    int Y;
    int numIB;
    int numOB;
    int numGIB;
    int numPE;
    int numNode;
    int rows;
    int cols;
    std::set<int> ibs;
    std::set<int> obs;
    std::set<int> gibs;
    std::set<int> pes;
    std::string arch_type;
    ADG* _adg; // generated ADG
    std::vector<DFG*> _dfgs; // from outside, not delete here
    std::vector<std::map<int, int>> _nodemaps;
    std::vector<std::map<int, int>> _imaps;
    std::vector<std::map<int, int>> _omaps;
    std::vector<std::set<int>> _linksets;
    std::vector<std::set<int>> _switchsets;
    std::vector<std::map<int, std::pair<int, std::string>>> _nodesets;
    // shortest distance between two ADG nodes (GPE nodes)
    std::map<std::pair<int, int>, int> _adgNode2NodeDist; // <<node-id, node-id>, dist>

protected:
    std::vector<Mapping*> _mappings;
    // mapping timeout in ms
    double _timeout; 
    // mapping start time point
    std::chrono::time_point<std::chrono::steady_clock> _start;
    // number of the candidate mapping ADG nodes of each DFG node in the _mapping->dfg
    // std::map<int, int> candidatesCnt; // <dfgnode-id, count>
    // std::map<int, std::vector<ADGNode*>> candidates; // <dfgnode-id, vector<adgnode>>
    // the DFG node IDs in placing order
    std::vector<std::vector<int>> dfgNodeIdPlaceOrders;

public:
    Generator(){}
    Generator(std::vector<DFG*> dfgs)   { _dfgs = dfgs; }
    ~Generator();

    std::vector<Mapping*> getMappingResult()    { return _mappings; }
    // set DFG and initialize DFG
    // modify: if the DFG is a modified one
    void setDFG(std::vector<DFG*> dfgs) { _dfgs = dfgs; }
    void addDFG(DFG* dfg) { _dfgs.emplace_back(dfg); }
    std::vector<DFG*> getDFGs(){ return _dfgs; }
    DFG* getDFG(int i){ return _dfgs[i]; }
    // get ADG
    ADG* getADG(){ return _adg; }
    // generate sub ADG of an ADG node
    ADG* generateSubADG(ADGNode*& node);
    // initialize mapping status of ADG
    void initializeAdg();
    // initialize mapping status of DFG
    void initializeDfg();
    // calculate the shortest path among ADG nodes
    void calAdgNodeDist();
    // get the shortest distance between two ADG nodes
    int getAdgNodeDist(int srcId, int dstId);

    std::vector<std::set<int>> linksets() { return _linksets; }
    std::vector<std::set<int>> switchsets() { return _switchsets; }
    std::vector<std::map<int, std::pair<int, std::string>>> nodesets() { return _nodesets; }
    std::set<int> linkset(int i) { return _linksets[i]; }
    std::set<int> switchset(int i) { return _switchsets[i]; }
    std::map<int, std::pair<int, std::string>> nodeset(int i) { return _nodesets[i]; }
    // // get the shortest distance between ADG node and ADG input
    // int getAdgNode2InputDist(int id);
    // // get the shortest distance between ADG node and ADG input
    // int getAdgNode2OutputDist(int id);
    // initialize candidates of DFG nodes
    // void initializeCandidates();
    // calculate the number of the candidates for one DFG node
    int calCandidatesCnt(DFGNode* dfgNode, int maxCandidates);
    // sort the DFG node IDs in placing order
    void sortDfgNodeInPlaceOrder();

    // timestamp functions
    void setStartTime();
    void setTimeOut(double timeout);
    double getTimeOut(){ return _timeout; }
    //get the running time in millisecond
    double runningTimeMS();

    
    bool execute(int , int , int , int , int , std::string );
    void analyzescale();
    void formalize(int , int , int , int , int );
    ADGLink* getlink(int& , ADGNode*& , ADGNode*& );
    void placenode(int , DFG* const& , std::set<int>& , std::map<int, int>& , std::set<int>& , std::set<int>& , std::set<int>& );
    void getinputnodes(int& , std::set<int>& , std::set<int>& );
    void getoutputnodes(int& , std::set<int>& , std::set<int>& );
    int countinodes(const int& );
    int countonodes(const int& , std::set<int>& );
    int getnearpe(int& , bool );
    int getneario(int& , bool );
    void placeins(DFG* const& , std::map<int, int>& , std::map<int, int>& , std::set<int>& , std::set<int>& , std::set<int>& );
    void placeouts(DFG* const& , std::map<int, int>& , std::map<int, int>& , std::set<int>& , std::set<int>& , std::set<int>& );
    bool findconnection(int& , int& , int& , int& , std::set<int>& , std::set<int>& );
    void addconnection(int& , int& , int& , int& , int& , std::set<int>& , std::set<int>& );

    // get max latency of the mapped DFG
    int getMaxLat();

};







#endif