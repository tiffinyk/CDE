
#include "generator/generator.h"


Generator::~Generator(){
    for(const auto& mapping : _mappings){
        if(mapping != nullptr){
            delete mapping;
        }
    }
}

// // set ADG and initialize ADG
// void Generator::setADG(ADG* adg){ 
//     _adg = adg; 
//     initializeAdg();
// }


// initialize mapping status of ADG
void Generator::initializeAdg(){
    // std::cout << "Initialize ADG\n";
    calAdgNodeDist();
}


// initialize mapping status of DFG
void Generator::initializeDfg(){
    // topoSortDfgNodes();
    for(auto& dfg: _dfgs){
        dfg->topoSortNodes();
    }    
}


// // initialize candidates of DFG nodes
// void Generator::initializeCandidates(){
//     Candidate cdt(_mapping, 50);
//     cdt.findCandidates();
//     // candidates = cdt.candidates(); // RETURN &
//     candidatesCnt = cdt.cnt();
// }


// // sort dfg nodes in reversed topological order
// // depth-first search
// void Generator::dfs(DFGNode* node, std::map<int, bool>& visited){
//     int nodeId = node->id();
//     if(visited.count(nodeId) && visited[nodeId]){
//         return; // already visited
//     }
//     visited[nodeId] = true;
//     for(auto& in : node->inputs()){
//         int inNodeId = in.second.first;
//         if(inNodeId == _dfg->id()){ // node connected to DFG input port
//             continue;
//         }
//         dfs(_dfg->node(inNodeId), visited); // visit input node
//     }
//     dfgNodeTopo.push_back(_dfg->node(nodeId));
// }

// // sort dfg nodes in reversed topological order
// void Generator::topoSortDfgNodes(){
//     std::map<int, bool> visited; // node visited status
//     for(auto& in : _dfg->outputs()){
//         int inNodeId = in.second.first;
//         if(inNodeId == _dfg->id()){ // node connected to DFG input port
//             continue;
//         }
//         dfs(_dfg->node(inNodeId), visited); // visit input node
//     }
// }


// calculate the shortest path among ADG nodes
void Generator::calAdgNodeDist(){
    // map ADG node id to continuous index starting from 0
    std::map<int, int> _adgNodeId2Idx;
    // distances among ADG nodes
    std::vector<std::vector<int>> _adgNodeDist; // [node-idx][node-idx]
    int i = 0;
    // if the ADG node with the index is GIB
    std::map<int, bool> adgNodeIdx2GIB;
    for(auto& node : _adg->nodes()){
        adgNodeIdx2GIB[i] = (node.second->type() == "GIB");
        _adgNodeId2Idx[node.first] = i++;
    }
    int n = i; // total node number
    int inf = 0x7fffffff;
    _adgNodeDist.assign(n, std::vector<int>(n, inf));
    for(auto& node : _adg->nodes()){
        int idx = _adgNodeId2Idx[node.first];
        _adgNodeDist[idx][idx] = 0;
        for(auto& src : node.second->inputs()){
            int srcId = src.second.first;
            if(srcId == _adg->id()){
                continue; // connected to ADG input port
            }
            int srcPort = src.second.second;
            ADGNode* srcNode = _adg->node(srcId);
            int dist = 1;
            if(srcNode->type() == "GIB" && node.second->type() == "GIB"){
                if(srcNode->outReged(srcPort)){ // output port reged
                    dist = 2;
                }
            }
            int srcIdx = _adgNodeId2Idx[srcId];
            _adgNodeDist[srcIdx][idx] = dist;
        }
    }
    // Floyd algorithm
    for (int k = 0; k < n; ++k) {
        if(adgNodeIdx2GIB[k]){
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (_adgNodeDist[i][k] < inf && _adgNodeDist[k][j] < inf &&
                        _adgNodeDist[i][j] > _adgNodeDist[i][k] + _adgNodeDist[k][j]) {
                        _adgNodeDist[i][j] = _adgNodeDist[i][k] + _adgNodeDist[k][j];
                    }
                }
            }
        }        
    }

    // shortest distance between two ADG nodes (GPE/IOB nodes)
    for(auto& inode : _adg->nodes()){
        if(inode.second->type() == "GIB" || inode.second->type() == "OB"){
            continue;
        }
        int i = _adgNodeId2Idx[inode.first];
        for(auto& jnode : _adg->nodes()){
            if(jnode.second->type() == "GIB" || jnode.second->type() == "IB" ||
              (inode.second->type() == "IB" && jnode.second->type() == "OB")){
                continue;
            }
            int j = _adgNodeId2Idx[jnode.first];
            _adgNode2NodeDist[std::make_pair(inode.first, jnode.first)] = _adgNodeDist[i][j];
            // std::cout << inode.first << "," << jnode.first << ": " << _adgNodeDist[i][j] << "  ";
        }
        // std::cout << std::endl;
    }

    // // shortest distance between ADG node (GPE node) and the ADG IO
    // for(auto& inode : _adg->nodes()){
    //     if(inode.second->type() != "GPE"){
    //         continue;
    //     }
    //     int i = _adgNodeId2Idx[inode.first];
    //     int minDist2IB = inf;
    //     int minDist2OB = inf;
    //     for(auto& jnode : _adg->nodes()){            
    //         auto jtype = jnode.second->type();
    //         int j = _adgNodeId2Idx[jnode.first];
    //         if(jtype == "IB"){                
    //             minDist2IB = std::min(minDist2IB, _adgNodeDist[j][i]);
    //         }else if(jtype == "OB"){
    //             minDist2OB = std::min(minDist2OB, _adgNodeDist[i][j]);
    //         }                       
    //     }
    //     _adgNode2IODist[inode.first] = std::make_pair(minDist2IB, minDist2OB);
    //     // std::cout << inode.first << ": " << minDist2IB << "," << minDist2OB << std::endl;
    // }
}


// get the shortest distance between two ADG nodes
int Generator::getAdgNodeDist(int srcId, int dstId){
    // return _adgNodeDist[_adgNodeId2Idx[srcId]][_adgNodeId2Idx[dstId]];
    return _adgNode2NodeDist[std::make_pair(srcId, dstId)];
}

// // get the shortest distance between ADG node and ADG input
// int Generator::getAdgNode2InputDist(int id){
//     return _adgNode2IODist[id].first;
// }

// // get the shortest distance between ADG node and ADG input
// int Generator::getAdgNode2OutputDist(int id){
//     return _adgNode2IODist[id].second;
// }


// calculate the number of the candidates for one DFG node
int Generator::calCandidatesCnt(DFGNode* dfgNode, int maxCandidates){
    int candidatesCnt = 0;
    for(auto& elem : _adg->nodes()){
        auto adgNode = elem.second;
        //select GPE node
        if(adgNode->type() != "GPE"){  
            continue;
        }
        // check if the DFG node operationis supported
        if(adgNode->opCapable(dfgNode->operation())){
            candidatesCnt++;
        }
    }
    return std::min(candidatesCnt, maxCandidates);
}

// sort the DFG node IDs in placing order
void Generator::sortDfgNodeInPlaceOrder(){
    std::map<int, int> candidatesCnt; // <dfgnode-id, count>
    dfgNodeIdPlaceOrders.clear();
    // topological order
    for(auto& dfg : _dfgs){
        std::vector<int> dfgNodeIdPlaceOrder;
        for(auto node : dfg->topoNodes()){ 
            dfgNodeIdPlaceOrder.emplace_back(node->id());
            // std::cout << node->id() << ", ";
            int cnt = calCandidatesCnt(node, 50);
            candidatesCnt[node->id()] = cnt;
        }
        dfgNodeIdPlaceOrders.emplace_back(dfgNodeIdPlaceOrder);
    }
    // std::cout << std::endl;
    // sort DFG nodes according to their candidate numbers
    // std::random_shuffle(dfgNodeIds.begin(), dfgNodeIds.end()); // randomly sort will cause long routing paths
    /*std::stable_sort(dfgNodeIdPlaceOrder.begin(), dfgNodeIdPlaceOrder.end(), [&](int a, int b){
        return candidatesCnt[a] <  candidatesCnt[b];
    });*/
}


// ===== timestamp functions >>>>>>>>>
void Generator::setStartTime(){
    _start = std::chrono::steady_clock::now();
}


void Generator::setTimeOut(double timeout){
    _timeout = timeout;
}


//get the running time in millisecond
double Generator::runningTimeMS(){
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end-_start).count();
}

//get CGRA ADG scale and generate ADG nodes
void Generator::analyzescale()
{
    int numop = 1;
    int numin = 1;
    int numout = 1;
    for(const auto& dfg : _dfgs){
        if(dfg->nodes().size() > numop){
            numop = dfg->nodes().size();
        }
        if(dfg->inputs().size() > numin){
            numin = dfg->inputs().size();
        }
        if(dfg->outputs().size() > numout){
            numout = dfg->outputs().size();
        }
    }
    if(numin > numout){
        numout = numin;
    }
    else{
        numin = numout;
    }
    rows = numin % 2 == 0 ? numin >> 1 : (numin >> 1) + 1;
    cols = numop % rows == 0 ? numop / rows : numop / rows + 1;
    numPE = rows * cols;
    numIB = rows << 1;
    numOB = rows << 1;
    if(arch_type == "direct"){
        Y = rows;
        X = cols + 4;
    }
    else if(arch_type == "GIB"){
        numGIB = (rows + 1) * (cols + 1);
        Y = (rows << 1) + 1;
        X = (cols << 1) + 5;
    }

    //generate ADG nodes
    int id = 0;
    _adg = new ADG;
    _adg->setId(id++);
    std::vector<int> switches;
    if(arch_type == "direct"){
    for(int x = 0; x < X; x++){
        for(int y = 0; y < Y; y++){
            ADGNode* node = new ADGNode(id);
            node->setX(x);
            node->setY(y);
            if(x == 0 || x == X - 1){
                node->setType("IB");
                ibs.emplace(id);
            }
            else if(x == 1 || x == X - 2){
                node->setType("OB");
                obs.emplace(id);
            }
            else{
                node->setType("GPE");
                pes.emplace(id);
            }
            _adg->addNode(id++, node);
        }
    }
    }
    else if(arch_type == "GIB"){
    for(int x = 0; x < X; x++){
        for(int y = 0; y < Y; y++){
            if((x == 0 || x == X - 1) && y % 2 == 1){
                ADGNode* node = new ADGNode(id);
                node->setX(x);
                node->setY(y);
                node->setType("IB");
                ibs.emplace(id);
                _adg->addNode(id++, node);
            }
            else if((x == 1 || x == X - 2) && y % 2 == 1){
                ADGNode* node = new ADGNode(id);
                node->setX(x);
                node->setY(y);
                node->setType("OB");
                obs.emplace(id);
                _adg->addNode(id++, node);
            }
            else if(x > 1 && x < X - 2 && x % 2 == 1 && y % 2 == 1){
                ADGNode* node = new ADGNode(id);
                node->setX(x);
                node->setY(y);
                node->setType("GPE");
                pes.emplace(id);
                _adg->addNode(id++, node);
            }
            else if(x > 1 && x < X - 2 && x % 2 == 0 && y % 2 == 0){
                ADGNode* node = new ADGNode(id);
                node->setX(x);
                node->setY(y);
                node->setType("GIB");
                gibs.emplace(id);
                _adg->addNode(id++, node);
            }
        }
    }
    }
    numNode = _adg->nodes().size();
    std::cout << "pe cols: " << cols << ", pe rows: " << rows << ", X: " << X << ", Y: " << Y << ", node number: " << numNode << std::endl;
    //std::cout << "numPE: " << numPE << ", numI/OB: " << numIB << std::endl;
}

int Generator::getnearpe(int& io, bool isinput)
{
    int nearpe;
    int midnode = numNode >> 1;
    if(arch_type == "direct"){
        int offset = isinput ? Y << 1 : Y;
        nearpe = io > midnode ? io - offset : io + offset;
    }
    else if(arch_type == "GIB"){
        int offset = isinput ? rows + Y : Y;
        nearpe = io > midnode ? io - offset : io + offset;
    }
    return nearpe;
}

int Generator::getneario(int& pe, bool isinput)
{
    int neario;
    int midnode = numNode >> 1;
    if(arch_type == "direct"){
        if(isinput) { neario = pe > midnode ? (pe - 1) % Y + 1 + (X - 1) * Y : (pe - 1) % Y + 1; }
        else { neario = pe > midnode ? (pe - 1) % Y + 1 + (X - 2) * Y : (pe - 1) % Y + 1 + Y; }
    }
    else if(arch_type == "GIB"){
        if(isinput) { neario = pe > midnode ? numNode - rows * 2 + pe % Y : pe % Y - rows; }
        else { neario = pe > midnode ? numNode - rows * 3 + pe % Y : pe % Y; }
    }
    return neario;
}

void Generator::placeins(DFG* const& dfg, std::map<int, int>& imap, std::map<int, int>& nodemap, std::set<int>& dfgnodes, std::set<int>& ibs, std::set<int>& penodes)
{
    for(const auto& dfginput: dfg->inputs()){
    int dfgportid = dfginput.first;
    int ibcandidate = dfgportid < rows ? *ibs.begin() : *ibs.rbegin();
    int midnode = numNode >> 1;
    int nearpe = getnearpe(ibcandidate, true);
    for(const auto& it : dfginput.second){
        int dfgnodeid = it.first;
        int nearib;
        if(dfgnodes.count(dfgnodeid) > 0){
            int pecandidate = dfgportid < rows ? *penodes.begin() : *penodes.rbegin();
            nearib = getneario(pecandidate, true);
            if(ibs.count(nearib) > 0){
                ibcandidate = nearib;
            }
            else if(penodes.count(nearpe) > 0){
                pecandidate = nearpe;
            }
            nodemap[dfgnodeid] = pecandidate;
            dfgnodes.erase(dfgnodeid);
            penodes.erase(pecandidate);
            std::string operation = dfg->node(dfgnodeid)->operation();
            if(!_adg->node(pecandidate)->opCapable(operation)) { _adg->node(pecandidate)->addOperation(operation); }
            std::cout << "(i) dfg node: " << dfgnodeid << ", adg pe node: " << pecandidate << std::endl;
        }
        else{
            int peid = nodemap[dfgnodeid];
            nearib = getneario(peid, true);
            std::cout << "(i) dfg node: " << dfgnodeid << ", mapped adg pe node: " << peid << std::endl;
            if(ibs.count(nearib) > 0){
                ibcandidate = nearib;
            }
        }
    }
    imap[dfgportid] = ibcandidate;
    ibs.erase(ibcandidate);
    std::cout << "dfg port: " << dfgportid << ", ib: " << ibcandidate << std::endl;
    }
}

void Generator::placeouts(DFG* const& dfg, std::map<int, int>& omap, std::map<int, int>& nodemap, std::set<int>& dfgnodes, std::set<int>& obs, std::set<int>& penodes)
{
    for(const auto& dfgoutput: dfg->outputs()){
    int dfgportid = dfgoutput.first;
    int dfgnodeid = dfgoutput.second.first;
    int obcandidate = dfgportid < rows ? *obs.begin() : *obs.rbegin();
    int midnode = numNode >> 1;
    int nearpe = getnearpe(obcandidate, false);
    int nearob;
    if(dfgnodes.count(dfgnodeid) > 0){
        int pecandidate = dfgportid < rows ? *penodes.begin() : *penodes.rbegin();        
        nearob = getneario(pecandidate, false);
        if(obs.count(nearob) > 0){
            obcandidate = nearob;
        }
        else if(penodes.count(nearpe) > 0){
            pecandidate = nearpe;
        }
        nodemap[dfgnodeid] = pecandidate;
        dfgnodes.erase(dfgnodeid);
        penodes.erase(pecandidate);
        std::string operation = dfg->node(dfgnodeid)->operation();
        if(!_adg->node(pecandidate)->opCapable(operation)) { _adg->node(pecandidate)->addOperation(operation); }
        std::cout << "(i) dfg node: " << dfgnodeid << ", adg pe node: " << pecandidate << std::endl;
    }
    else{
        int peid = nodemap[dfgnodeid];
        nearob = getneario(peid, false);
        std::cout << "(i) dfg node: " << dfgnodeid << ", mapped adg pe node: " << peid << std::endl;
        if(obs.count(nearob) > 0){
            obcandidate = nearob;
        }
    }
    
    omap[dfgportid] = obcandidate;
    obs.erase(obcandidate);
    std::cout << "dfg port: " << dfgportid << ", ob: " << obcandidate << std::endl;
    }
}

int Generator::countinodes(const int& n)
{
    int number = 0;
    if(arch_type == "direct"){
        for(const auto& it : _adg->node(n)->inputs()){
            if(ibs.count(it.second.first) == 0) { number++; }
        }
    }
    else if(arch_type == "GIB"){
        for(const auto& it : _adg->node(n)->inputs()){
            int cid = it.second.first;
            if(gibs.count(cid) > 0){
                for(const auto& s : _adg->node(cid)->inputs()){
                    if(ibs.count(s.second.first) == 0 && gibs.count(s.second.first) == 0) { number++; }
                }
            }
        }
    }
    return number;
}

int Generator::countonodes(const int& n, std::set<int>& obnodes)
{
    int number = 0;
    if(arch_type == "direct"){
        for(const auto& it : _adg->node(n)->outputs()){
            bool toPE = false;
            for(const auto& to : it.second){
                if(obs.count(to.first) == 0){
                    toPE = true;
                }
                else if(obnodes.count(to.first) == 0){
                    toPE = false;
                    break;
                }
            }
            if(toPE == true) { number++; }
        }
    }
    else if(arch_type == "GIB"){
        for(const auto& it : _adg->node(n)->outputs()){
            for(const auto& to : it.second){
                if(gibs.count(to.first) > 0){
                    for(const auto& ds : _adg->node(to.first)->outputs()){
                        bool toPE = false;
                        for(const auto& d : ds.second){
                            if(obs.count(d.first) == 0){
                                toPE = true;
                            }
                            else if(obnodes.count(d.first) == 0){
                                toPE = false;
                                break;
                            }
                        }
                        if(toPE == true) { number++; }
                    }
                }
            }
        }
    }
    return number;
}

void Generator::getinputnodes(int& n, std::set<int>& inputnodes, std::set<int>& penodes)
{
    if(arch_type == "direct"){
    for(const auto& it : _adg->node(n)->inputs()){
        if(penodes.count(it.second.first) > 0){
            inputnodes.emplace(it.second.first);
        }
    }
    }
    else if(arch_type == "GIB"){
    for(const auto& it : _adg->node(n)->inputs()){
        int cid = it.second.first;
        if(gibs.count(cid) > 0){
            for(const auto& s : _adg->node(cid)->inputs()){
                if(penodes.count(s.second.first) > 0){
                    inputnodes.emplace(s.second.first);
                }
            }
        }
    }
    }
}

void Generator::getoutputnodes(int& n, std::set<int>& outputnodes, std::set<int>& penodes)
{
    if(arch_type == "direct"){
    for(const auto& it : _adg->node(n)->outputs()){
        for(const auto& o : it.second){
            if(penodes.count(o.first) > 0){
                outputnodes.emplace(o.first);
            }
        }
    }
    }
    else if(arch_type == "GIB"){
    for(const auto& it : _adg->node(n)->outputs()){
        for(const auto& o : it.second){
            if(gibs.count(o.first) > 0){
                for(const auto& ds : _adg->node(o.first)->outputs()){
                    bool used = false;
                    std::set<int> temp;
                    for(const auto& d : ds.second){
                        if(penodes.count(d.first) > 0){
                            temp.emplace(d.first);
                        }
                        else{
                            used = true;
                            break;
                        }
                    }
                    if(used == false && temp.size() > 0){
                        for(auto& i : temp){
                            outputnodes.emplace(i);
                        }
                    }
                }
            }
        }
    }
    }
}

void Generator::placenode(int dfgnodeid, DFG* const& dfg, std::set<int>& dfgnodes, std::map<int, int>& nodemap, std::set<int>& candidatenodes, std::set<int>& penodes, std::set<int>& obnodes)
{
    int numInputs = dfg->node(dfgnodeid)->numInputs();
    int numOutputs = dfg->node(dfgnodeid)->numOutputs();
    std::string operation = dfg->node(dfgnodeid)->operation();
    std::set<int>& candidates = candidatenodes.empty() ? penodes : candidatenodes;
    int candidate = *candidates.begin();
    int minscore = dfg->edges().size();
    int level = 0;
    for(const auto& i : candidates){
        int numInports = countinodes(i); //ADG node inports number (except from IB)
        int numOutports = countonodes(i, obnodes); //ADG node outports number (except to OB)
        int din = numInports - numInputs;
        int dout = numOutports - numOutputs;
        int score = abs(din) + abs(dout);
        /*if(level < 1 && score < minscore){
            candidate = i;
            minscore = score;
        }*/
        if(level < 1 && (din > 0 || dout > 0)){
            candidate = i;
            level = 1;
            minscore = 3;
        }
        if(level < 2 && din > 0 && dout > 0){
            candidate = i;
            level = 2;
            minscore = 3;
        }
        if(din > 0 && dout > 0 && score < minscore){
            candidate = i;
            minscore = score;
            level = 2;
        }
        if(din == 0 && dout == 0){
            candidate = i;
            break;
        }
    }
    nodemap[dfgnodeid] = candidate;
    dfgnodes.erase(dfgnodeid);
    penodes.erase(candidate);
    if(!candidatenodes.empty()) { candidatenodes.erase(candidate); }
    if(!_adg->node(candidate)->opCapable(operation)) { _adg->node(candidate)->addOperation(operation); }
    std::set<int> adgnodeinputs;
    getinputnodes(candidate, adgnodeinputs, penodes);
    std::cout << "dfg node: " << dfgnodeid << ", adg pe node: " << candidate << std::endl;
    for(const auto& input : dfg->node(dfgnodeid)->inputs()){
        int inputid = input.second.first;
        if(inputid > 0 && dfgnodes.count(inputid) > 0){
            placenode(inputid, dfg, dfgnodes, nodemap, adgnodeinputs, penodes, obnodes);
        }
    }
    std::set<int> adgnodeoutputs;
    getoutputnodes(candidate, adgnodeoutputs, penodes);
    for(const auto& outport : dfg->node(dfgnodeid)->outputs()){
        for(const auto& output : outport.second){
            int outputid = output.first;
            if(outputid > 0 && dfgnodes.count(outputid) > 0){
                placenode(outputid, dfg, dfgnodes, nodemap, adgnodeoutputs, penodes, obnodes);
            }
        }
    }    
}

bool Generator::findconnection(int& srcId, int& srcPortIdx, int& dstId, int& dstPortIdx, std::set<int>& usedlinks, std::set<int>& usedgibs)
{
    bool found = false;
    if(arch_type == "direct"){
            for(auto& link : _adg->links()){
                if(srcId == link.second->srcId() && srcPortIdx == link.second->srcPortIdx() && dstId == link.second->dstId()){
                    found = true;
                    std::cout << "ADG link found." << std::endl;
                    usedlinks.emplace(link.first);
                }
            }
    }
    else if(arch_type == "GIB"){
        std::set<int> visited;
        std::queue<std::pair<int, int>> nodeQue;
        std::map<int, std::vector<std::pair<int, int>>> paths;
        nodeQue.emplace(std::make_pair(dstId, dstPortIdx));
        while(nodeQue.empty() == false){
            int cid = nodeQue.front().first;
            int cport = nodeQue.front().second;
            ADGNode* current_node = _adg->node(cid);
            nodeQue.pop();
            visited.emplace(cid);
            if(current_node->type() == "GIB"){
            for(const auto& it : current_node->out2ins(cport)){
                bool used = false;
                for(const auto& lid : usedlinks){
                    ADGLink* l = _adg->link(lid);
                    if(l->dstId() == cid && l->dstPortIdx() == it){
                        used = true;
                        break;
                    }
                }
                if(used == true){
                    continue;
                }
                int sid = current_node->input(it).first;
                int spid = current_node->input(it).second;
                std::vector<std::pair<int, int>> path;
                if(cid != dstId) { path = paths[cid]; }
                path.emplace_back(std::make_pair(cid, it));
                paths[sid] = path;
                if(sid == srcId){
                    found = true;
                    srcPortIdx = spid;
                    break;
                }
                else if(_adg->node(sid)->type() == "GIB" && visited.count(sid) == 0){
                    nodeQue.emplace(std::make_pair(sid, spid));
                }
            }
            }
            else{
            for(const auto& it : current_node->inputs()){
                bool used = false;
                int pid = it.first;
                for(const auto& lid : usedlinks){
                    ADGLink* l = _adg->link(lid);
                    if(l->dstId() == cid && l->dstPortIdx() == pid){
                        used = true;
                        break;
                    }
                }
                if(used == true){
                    continue;
                }
                int sid = it.second.first;
                int spid = it.second.second;
                std::vector<std::pair<int, int>> path;
                if(cid != dstId) { path = paths[cid]; }
                path.emplace_back(std::make_pair(cid, pid));
                paths[sid] = path;
                if(sid == srcId){
                    found = true;
                    srcPortIdx = spid;
                    break;
                }
                else if(_adg->node(sid)->type() == "GIB" && visited.count(sid) == 0){
                    nodeQue.emplace(std::make_pair(sid, spid));
                }
            }
            }
            if(found == true){
                break;
            }
        }
        if(found == true){
            for(const auto& it : paths[srcId]){
                int d = it.first;
                int dport = it.second;
                for(auto& link : _adg->links()){
                    if(d == link.second->dstId() && dport == link.second->dstPortIdx()){
                        usedlinks.emplace(link.first);
                    }
                }
                if(_adg->node(d)->type() == "GIB" && usedgibs.count(d) == 0){
                    usedgibs.emplace(d);
                }
            }
        }
    }
    return found;
}

void Generator::addconnection(int& id, int& srcId, int& srcPortIdx, int& dstId, int& dstPortIdx, std::set<int>& usedlinks, std::set<int>& usedgibs)
{
    if(arch_type == "direct"){
                dstPortIdx = _adg->node(dstId)->inputs().size();
                ADGLink* l = new ADGLink(id);
                l->setSrcId(srcId);
                l->setSrcPortIdx(srcPortIdx);
                l->setDstId(dstId);
                l->setDstPortIdx(dstPortIdx);
                _adg->addLink(id, l);
                usedlinks.emplace(id++);
    }
    else if(arch_type == "GIB"){
        bool pathfound = false;
        std::set<int> visited;
        std::queue<std::pair<int, int>> nodeQue;
        std::map<int, std::vector<std::pair<int, std::pair<int, int>>>> paths; // <node id, path<<node id, <inport id, outport id>>>>
        nodeQue.emplace(std::make_pair(dstId, dstPortIdx));
        while(nodeQue.empty() == false){
            int cid = nodeQue.front().first;
            int cport = nodeQue.front().second;
            ADGNode* current_node = _adg->node(cid);
            nodeQue.pop();
            visited.emplace(cid);
            for(const auto& it : current_node->inputs()){
                bool used = false;
                int pid = it.first;
                for(const auto& lid : usedlinks){
                    ADGLink* l = _adg->link(lid);
                    if(l->dstId() == cid && l->dstPortIdx() == pid){
                        used = true;
                        break;
                    }
                }
                if(used == true){
                    continue;
                }
                int sid = it.second.first;
                int spid = it.second.second;
                std::vector<std::pair<int, std::pair<int, int>>> path;
                if(cid != dstId) { path = paths[cid]; }
                auto port_pair = std::make_pair(pid, cport);
                path.emplace_back(std::make_pair(cid, port_pair));
                paths[sid] = path;
                if(sid == srcId){
                    pathfound = true;
                    srcPortIdx = spid;
                    break;
                }
                else if(_adg->node(sid)->type() == "GIB" && visited.count(sid) == 0){
                    nodeQue.emplace(std::make_pair(sid, spid));
                }
            }
            if(pathfound == true){
                break;
            }
        }
        if(pathfound == true){ //only need to add GIB inner connections
            std::cout << "path found." << std::endl;
            for(const auto& it : paths[srcId]){
                int d = it.first;
                int diport = it.second.first;
                int doport = it.second.second;
                for(auto& link : _adg->links()){
                    if(d == link.second->dstId() && diport == link.second->dstPortIdx()){
                        usedlinks.emplace(link.first);
                    }
                }
                ADGNode* n = _adg->node(d);
                if(n->type() == "GIB" && n->out2ins(doport).count(diport) == 0){
                    n->addOut2ins(doport, diport);
                }
                if(n->type() == "GIB" && usedgibs.count(d) == 0){
                    usedgibs.emplace(d);
                }
            }
        }
        else{
            ADGNode* srcNode = _adg->node(srcId);
            ADGNode* dstNode = _adg->node(dstId);
            int sx = srcNode->x();
            int sy = srcNode->y();
            int dx = dstNode->x();
            int dy = dstNode->y();
            int midNode = numNode >> 1;
            std::string stype = srcNode->type();
            std::string dtype = dstNode->type();
            int gs = Y + 1, gd = Y + 1, gc = Y + 1;
            int gsinport, gsoutport, gdinport, gdoutport, gcinport, gcoutport;
            if(stype == "IB"){
                if(dy > sy){
                    if(dx > sx){
                        gs = srcId + Y;
                        gd = dstId - rows - 1;
                    }
                    else if(dx < sx){
                        gs = srcId - Y + 1;
                        gd = dstId + rows;
                    }
                }
                else if(dy < sy){
                    if(dx > sx){
                        gs = srcId + Y - 1;
                        gd = dstId - rows;
                    }
                    else if(dx < sx){
                        gs = srcId - Y;
                        gd = dstId + rows + 1;
                    }
                }
                else{
                    if(dx > sx){
                        gs = srcId + Y;
                        gd = dstId - rows;
                    }
                    else{
                        gs = srcId - Y;
                        gd = dstId + rows;
                    }
                }
            }
            else{
                if(dy > sy){
                    if(dx > sx){
                        gs = srcId + rows + 1;
                        gd = dstId - rows - 1;
                    }
                    else if(dx < sx){
                        gs = srcId - rows;
                        gd = dstId + rows;
                    }
                    else{
                        gs = srcId - rows;
                        gd = dstId - rows - 1;
                    }
                }
                else if(dy < sy){
                    if(dx > sx){
                        gs = srcId + rows;
                        gd = dstId - rows;
                    }
                    else if(dx < sx){
                        gs = srcId - rows - 1;
                        gd = dstId + rows + 1;
                    }
                    else{
                        gs = srcId + rows;
                        gd = dstId + rows + 1;
                    }
                }
                else{
                    if(dx > sx){
                        gs = srcId + rows;
                        gd = dstId - rows - 1;
                    }
                    else{
                        gs = srcId - rows;
                        gd = dstId + rows + 1;
                    }
                }
            }
            std::cout << "gs: " << gs << ", gd: " << gd << ", gc: " << gc << std::endl;
            if(gs == gd){
                gc = gs;
                ADGNode* gibc = _adg->node(gc);
                bool link1found = false;
                bool link2found = false;
                for(auto& link : _adg->links()){
                    if(srcId == link.second->srcId() && gc == link.second->dstId()){
                        link1found = true;
                        srcPortIdx = link.second->srcPortIdx();
                        gcinport = link.second->dstPortIdx();
                        usedlinks.emplace(link.first);
                    }
                    else if(gc == link.second->srcId() && dstId == link.second->dstId() && usedlinks.count(link.first) == 0){
                        link2found = true;
                        gcoutport = link.second->srcPortIdx();
                        dstPortIdx = link.second->dstPortIdx();
                        usedlinks.emplace(link.first);
                    }
                }
                if(link1found == false){
                    gcinport = gibc->inputs().size();
                    ADGLink* l = new ADGLink(id);
                    l->setSrcId(srcId);
                    l->setSrcPortIdx(srcPortIdx);
                    l->setDstId(gc);
                    l->setDstPortIdx(gcinport);
                    _adg->addLink(id, l);
                    usedlinks.emplace(id++);
                }
                if(link2found == false){
                    gcoutport = gibc->outputs().size();
                    dstPortIdx = _adg->node(dstId)->inputs().size();
                    ADGLink* l = new ADGLink(id);
                    l->setSrcId(gc);
                    l->setSrcPortIdx(gcoutport);
                    l->setDstId(dstId);
                    l->setDstPortIdx(dstPortIdx);
                    _adg->addLink(id, l);
                    usedlinks.emplace(id++);
                }
                if(gibc->out2ins(gcoutport).count(gcinport) == 0){
                    gibc->addOut2ins(gcoutport, gcinport);
                }
                if(usedgibs.count(gc) == 0){
                    usedgibs.emplace(gc);
                }
            }
            else if(abs(gd - gs) < 9 || gd % Y == gs % Y){
                ADGNode* gibs = _adg->node(gs);
                ADGNode* gibd = _adg->node(gd);
                bool link1found = false;
                bool link2found = false;
                bool link3found = false;
                for(auto& link : _adg->links()){
                    if(srcId == link.second->srcId() && gs == link.second->dstId()){
                        link1found = true;
                        srcPortIdx = link.second->srcPortIdx();
                        gsinport = link.second->dstPortIdx();
                        usedlinks.emplace(link.first);
                    }
                    else if(gs == link.second->srcId() && gd == link.second->dstId() && usedlinks.count(link.first) == 0){ //vacant GIB track found
                        link2found = true;
                        gsoutport = link.second->srcPortIdx();
                        gdinport = link.second->dstPortIdx();
                        usedlinks.emplace(link.first);
                    }
                    else if(gd == link.second->srcId() && dstId == link.second->dstId() && usedlinks.count(link.first) == 0){ //vacant GIB track found
                        link3found = true;
                        gdoutport = link.second->srcPortIdx();
                        dstPortIdx = link.second->dstPortIdx();
                        usedlinks.emplace(link.first);
                    }
                }
                if(link1found == false){
                    gsinport = gibs->inputs().size();
                    ADGLink* l = new ADGLink(id);
                    l->setSrcId(srcId);
                    l->setSrcPortIdx(srcPortIdx);
                    l->setDstId(gs);
                    l->setDstPortIdx(gsinport);
                    _adg->addLink(id, l);
                    usedlinks.emplace(id++);
                }
                if(link2found == false){
                    gsoutport = gibs->outputs().size();
                    gdinport = gibd->inputs().size();
                    ADGLink* l = new ADGLink(id);
                    l->setSrcId(gs);
                    l->setSrcPortIdx(gsoutport);
                    l->setDstId(gd);
                    l->setDstPortIdx(gdinport);
                    _adg->addLink(id, l);
                    usedlinks.emplace(id++);
                }
                if(link3found == false){
                    gdoutport = gibd->outputs().size();
                    dstPortIdx = _adg->node(dstId)->inputs().size();
                    ADGLink* l = new ADGLink(id);
                    l->setSrcId(gd);
                    l->setSrcPortIdx(gdoutport);
                    l->setDstId(dstId);
                    l->setDstPortIdx(dstPortIdx);
                    _adg->addLink(id, l);
                    usedlinks.emplace(id++);
                }
                if(gibs->out2ins(gsoutport).count(gsinport) == 0){
                    gibs->addOut2ins(gsoutport, gsinport);
                }
                if(gibd->out2ins(gdoutport).count(gdinport) == 0){
                    gibd->addOut2ins(gdoutport, gdinport);
                }
                if(usedgibs.count(gs) == 0){
                    usedgibs.emplace(gs);
                }
                if(usedgibs.count(gd) == 0){
                    usedgibs.emplace(gd);
                }
            }
            else{
                ADGNode* gibs = _adg->node(gs);
                ADGNode* gibd = _adg->node(gd);
                gc = gs + Y * ((gibd->x()) - (gibs->x())) / 2;
                ADGNode* gibc = _adg->node(gc);
                bool link1found = false;
                bool link2found = false;
                bool link3found = false;
                bool link4found = false;
                for(auto& link : _adg->links()){
                    if(srcId == link.second->srcId() && gs == link.second->dstId()){
                        link1found = true;
                        srcPortIdx = link.second->srcPortIdx();
                        gsinport = link.second->dstPortIdx();
                        usedlinks.emplace(link.first);
                    }
                    else if(gs == link.second->srcId() && gc == link.second->dstId() && usedlinks.count(link.first) == 0){ //vacant GIB track found
                        link2found = true;
                        gsoutport = link.second->srcPortIdx();
                        gcinport = link.second->dstPortIdx();
                        usedlinks.emplace(link.first);
                    }
                    else if(gc == link.second->srcId() && gd == link.second->dstId() && usedlinks.count(link.first) == 0){ //vacant GIB track found
                        link3found = true;
                        gcoutport = link.second->srcPortIdx();
                        gdinport = link.second->dstPortIdx();
                        usedlinks.emplace(link.first);
                    }
                    else if(gd == link.second->srcId() && dstId == link.second->dstId() && usedlinks.count(link.first) == 0){ //vacant GIB track found
                        link4found = true;
                        gdoutport = link.second->srcPortIdx();
                        dstPortIdx = link.second->dstPortIdx();
                        usedlinks.emplace(link.first);
                    }
                }
                if(link1found == false){
                    gsinport = gibs->inputs().size();
                    ADGLink* l = new ADGLink(id);
                    l->setSrcId(srcId);
                    l->setSrcPortIdx(srcPortIdx);
                    l->setDstId(gs);
                    l->setDstPortIdx(gsinport);
                    _adg->addLink(id, l);
                    usedlinks.emplace(id++);
                }
                if(link2found == false){
                    gsoutport = gibs->outputs().size();
                    gcinport = gibc->inputs().size();
                    ADGLink* l = new ADGLink(id);
                    l->setSrcId(gs);
                    l->setSrcPortIdx(gsoutport);
                    l->setDstId(gc);
                    l->setDstPortIdx(gcinport);
                    _adg->addLink(id, l);
                    usedlinks.emplace(id++);
                }
                if(link3found == false){
                    gcoutport = gibc->outputs().size();
                    gdinport = gibd->inputs().size();
                    ADGLink* l = new ADGLink(id);
                    l->setSrcId(gc);
                    l->setSrcPortIdx(gcoutport);
                    l->setDstId(gd);
                    l->setDstPortIdx(gdinport);
                    _adg->addLink(id, l);
                    usedlinks.emplace(id++);
                }
                if(link4found == false){
                    gdoutport = gibd->outputs().size();
                    dstPortIdx = _adg->node(dstId)->inputs().size();
                    ADGLink* l = new ADGLink(id);
                    l->setSrcId(gd);
                    l->setSrcPortIdx(gdoutport);
                    l->setDstId(dstId);
                    l->setDstPortIdx(dstPortIdx);
                    _adg->addLink(id, l);
                    usedlinks.emplace(id++);
                }
                if(gibs->out2ins(gsoutport).count(gsinport) == 0){
                    gibs->addOut2ins(gsoutport, gsinport);
                }
                if(gibc->out2ins(gcoutport).count(gcinport) == 0){
                    gibc->addOut2ins(gcoutport, gcinport);
                }
                if(gibd->out2ins(gdoutport).count(gdinport) == 0){
                    gibd->addOut2ins(gdoutport, gdinport);
                }
                if(usedgibs.count(gs) == 0){
                    usedgibs.emplace(gs);
                }
                if(usedgibs.count(gc) == 0){
                    usedgibs.emplace(gc);
                }
                if(usedgibs.count(gd) == 0){
                    usedgibs.emplace(gd);
                }
            }
        }
    }
}

ADG* Generator::generateSubADG(ADGNode*& node)
{
    ADG* subADG = new ADG();
    int subnodeid = 0;
    int sublinkid = 0;
    subADG->setId(subnodeid);
    ADGNode* subnode;
    ADGLink* sublink;
    if(node->type() == "GPE"){
        subnode = new ADGNode(++subnodeid);
        subnode->setType("Const");
        subADG->addNode(subnodeid, subnode);
        int constid = subnodeid;

        subnode = new ADGNode(++subnodeid);
        subnode->setType("DelayPipe");
        subADG->addNode(subnodeid, subnode);
        int delaypipeid = subnodeid;

        subnode = new ADGNode(++subnodeid);
        subnode->setType("ALU");
        subADG->addNode(subnodeid, subnode);
        int aluid = subnodeid;

        subnode = new ADGNode(++subnodeid);
        subnode->setType("RF");
        subADG->addNode(subnodeid, subnode);
        int regid = subnodeid;

        for (int o = 0; o < node->numOperands(); o++) {
            subnode = new ADGNode(++subnodeid);
            subnode->setType("Muxn");
            subADG->addNode(subnodeid, subnode);
            int i;
            for(i = 0; i < node->numInputs(); i++){
                sublink = new ADGLink(sublinkid);
                sublink->setSrcId(0);
                sublink->setSrcPortIdx(i);
                sublink->setDstId(subnodeid);
                sublink->setDstPortIdx(i);
                subADG->addLink(sublinkid++, sublink);
            }

            sublink = new ADGLink(sublinkid);
            sublink->setSrcId(constid);
            sublink->setSrcPortIdx(0);
            sublink->setDstId(subnodeid);
            sublink->setDstPortIdx(i++);
            subADG->addLink(sublinkid++, sublink);

            sublink = new ADGLink(sublinkid);
            sublink->setSrcId(regid);
            sublink->setSrcPortIdx(1);
            sublink->setDstId(subnodeid);
            sublink->setDstPortIdx(i);
            subADG->addLink(sublinkid++, sublink);

            sublink = new ADGLink(sublinkid);
            sublink->setSrcId(subnodeid);
            sublink->setSrcPortIdx(0);
            sublink->setDstId(delaypipeid);
            sublink->setDstPortIdx(o);
            subADG->addLink(sublinkid++, sublink);

            sublink = new ADGLink(sublinkid);
            sublink->setSrcId(delaypipeid);
            sublink->setSrcPortIdx(o);
            sublink->setDstId(aluid);
            sublink->setDstPortIdx(o);
            subADG->addLink(sublinkid++, sublink);
        }

        sublink = new ADGLink(sublinkid);
        sublink->setSrcId(aluid);
        sublink->setSrcPortIdx(0);
        sublink->setDstId(regid);
        sublink->setDstPortIdx(0);
        subADG->addLink(sublinkid++, sublink);

        for(int o = 0; o < node->numOutputs(); o++){
            sublink = new ADGLink(sublinkid);
            sublink->setSrcId(regid);
            sublink->setSrcPortIdx(0);
            sublink->setDstId(0);
            sublink->setDstPortIdx(o);
            subADG->addLink(sublinkid++, sublink);
        }
    }
    else if(node->type() == "GIB"){
        for(int o = 0; o < node->numOutputs(); o++){
            int numin = node->out2ins(o).size();
            if(numin == 1){
                int i = *(node->out2ins(o).begin());
                sublink = new ADGLink(sublinkid);
                sublink->setSrcId(0);
                sublink->setSrcPortIdx(i);
                sublink->setDstId(0);
                sublink->setDstPortIdx(o);
                subADG->addLink(sublinkid++, sublink);
            }
            else if(numin > 1){
                subnode = new ADGNode(++subnodeid);
                subnode->setType("Muxn");
                subADG->addNode(subnodeid, subnode);
                int idx = 0;
                for (const auto& i : node->out2ins(o)) {
                    sublink = new ADGLink(sublinkid);
                    sublink->setSrcId(0);
                    sublink->setSrcPortIdx(i);
                    sublink->setDstId(subnodeid);
                    sublink->setDstPortIdx(idx);
                    subADG->addLink(sublinkid++, sublink);
                    idx++;
                }
                sublink = new ADGLink(sublinkid);
                sublink->setSrcId(subnodeid);
                sublink->setSrcPortIdx(0);
                sublink->setDstId(0);
                sublink->setDstPortIdx(o);
                subADG->addLink(sublinkid++, sublink);
            }
        }
    }
    else {
        if (node->numInputs() > 1) {
            for(int o = 0; o < node->numOutputs(); o++){
                subnode = new ADGNode(++subnodeid);
                subnode->setType("Muxn");
                subADG->addNode(subnodeid, subnode);
                for (int i = 0; i < node->numInputs(); i++) {
                    sublink = new ADGLink(sublinkid);
                    sublink->setSrcId(0);
                    sublink->setSrcPortIdx(i);
                    sublink->setDstId(subnodeid);
                    sublink->setDstPortIdx(i);
                    subADG->addLink(sublinkid++, sublink);
                }
                sublink = new ADGLink(sublinkid);
                sublink->setSrcId(subnodeid);
                sublink->setSrcPortIdx(0);
                sublink->setDstId(0);
                sublink->setDstPortIdx(o);
                subADG->addLink(sublinkid++, sublink);
            }
        }
        else {
            for(int o = 0; o < node->numOutputs(); o++){
                sublink = new ADGLink(sublinkid);
                sublink->setSrcId(0);
                sublink->setSrcPortIdx(0);
                sublink->setDstId(0);
                sublink->setDstPortIdx(o);
                subADG->addLink(sublinkid++, sublink);
            }
        }
    }

    for (auto& it : subADG->nodes()) {
        subnode = it.second;
        int numInputs = subnode->inputs().size() > 0 ? subnode->inputs().size() : 1;
        int numOutputs = subnode->outputs().size() > 0 ? subnode->outputs().size() : 1;
        subnode->setNumInputs(numInputs);
        subnode->setNumOutputs(numOutputs);
    }

    return subADG;
}

ADGLink* Generator::getlink(int& linkid, ADGNode*& srcnode, ADGNode*& dstnode)
{
    ADGLink* link = new ADGLink(linkid);
    int srcId = srcnode->id();
    int srcPortIdx = 0;
    int dstId = dstnode->id();
    int dstPortIdx = dstnode->inputs().size();
    link->setSrcId(srcId);
    link->setSrcPortIdx(srcPortIdx);
    link->setDstId(dstId);
    link->setDstPortIdx(dstPortIdx);
    return link;
}

void Generator::formalize(int bitWidth, int cfgDataWidth, int cfgAddrWidth, int cfgBlkOffset, int PEDelay)
{
    if(arch_type == "direct"){
    ADGNode *c, *e, *w, *n, *s, *in, *out;
    ADGLink *l;
    int middle = numNode >> 1;
    int linkid = _adg->links().size();
    int idx = 0, odx = 0;
    for(const auto& cid : ibs){
        //std::cout<< "check ib" << cid << std::endl;
        c = _adg->node(cid);
        if(c->outputs().size() == 0){
            if(cid > middle){
                w = _adg->node(cid - (Y << 1));
                l = getlink(linkid, c, w);
            }
            else{
                e = _adg->node(cid + (Y << 1));
                l = getlink(linkid, c, e);
            }
            _adg->addLink(linkid++, l);
        }
        l = new ADGLink(linkid);
        l->setSrcId(0);
        l->setSrcPortIdx(idx++);
        l->setDstId(cid);
        l->setDstPortIdx(0);
        _adg->addLink(linkid++, l);
    }
    for(const auto& cid : obs){
        //std::cout<< "check ob" << cid << std::endl;
        c = _adg->node(cid);
        if(c->inputs().size() == 0){
            if(cid > middle){
                w = _adg->node(cid - Y);
                l = getlink(linkid, w, c);
            }
            else{
                e = _adg->node(cid + Y);
                l = getlink(linkid, e, c);
            }
            _adg->addLink(linkid++, l);
        }
        l = new ADGLink(linkid);
        l->setSrcId(cid);
        l->setSrcPortIdx(0);
        l->setDstId(0);
        l->setDstPortIdx(odx++);
        _adg->addLink(linkid++, l);
    }
    for(const auto& cid : pes){
        c = _adg->node(cid);
        int x = c->x();
        int y = c->y();
        //std::cout<< "check pe" << cid << " (" << x << ", " << y << ")" << std::endl;
        if(c->inputs().size() == 0){
            if(x == 2 || x == X - 3){
                int i = cid > middle ? cid + (Y << 1) : cid - (Y << 1);
                //std::cout << "i: " << i << std::endl;
                in = _adg->node(i);
                l = getlink(linkid, in, c);
                _adg->addLink(linkid++, l);
            }
            else{
                if(y < Y - 1){
                    //std::cout << "is: " << cid + 1 << std::endl;
                    s = _adg->node(cid + 1);
                    l = getlink(linkid, s, c);
                    _adg->addLink(linkid++, l);
                }
                if(y > 0){
                    //std::cout << "in: " << cid - 1 << std::endl;
                    n = _adg->node(cid - 1);
                    l = getlink(linkid, n, c);
                    _adg->addLink(linkid++, l);
                }
                //std::cout << "iw: " << cid - Y << std::endl;
                w = _adg->node(cid - Y);
                l = getlink(linkid, w, c);
                _adg->addLink(linkid++, l);
                //std::cout << "ie: " << cid + Y << std::endl;
                e = _adg->node(cid + Y);
                l = getlink(linkid, e, c);
                _adg->addLink(linkid++, l);
            }
        }
        if(c->outputs().size() == 0){
            if(x == 2 || x == X - 3){
                int o = cid > middle ? cid + Y : cid - Y;
                //std::cout << "o: " << o << std::endl;
                out = _adg->node(o);
                l = getlink(linkid, c, out);
                _adg->addLink(linkid++, l);
            }
            else{
                if(y < Y - 1){
                    //std::cout << "os: " << cid + 1 << std::endl;
                    s = _adg->node(cid + 1);
                    l = getlink(linkid, c, s);
                    _adg->addLink(linkid++, l);
                }
                if(y > 0){
                    //std::cout << "on: " << cid - 1 << std::endl;
                    n = _adg->node(cid - 1);
                    l = getlink(linkid, c, n);
                    _adg->addLink(linkid++, l);
                }
                //std::cout << "ow: " << cid - Y << std::endl;
                w = _adg->node(cid - Y);
                l = getlink(linkid, c, w);
                _adg->addLink(linkid++, l);
                //std::cout << "oe: " << cid + Y << std::endl;
                e = _adg->node(cid + Y);
                l = getlink(linkid, c, e);
                _adg->addLink(linkid++, l);
            }
        }
    }
    }
    else if(arch_type == "GIB"){
    ADGNode *c, *gibnw, *gibsw, *gibne, *gibse;
    ADGLink *l;
    int middle = numNode >> 1;
    int linkid = _adg->links().size();
    int nwid, swid, neid, seid;
    int idx = 0, odx = 0;
    for(const auto& cid : ibs){
        //std::cout<< "check ib" << cid << std::endl;
        c = _adg->node(cid);
        if(cid > middle){
            nwid = cid - Y;
            swid = cid - Y + 1;
            bool nwconnect = false;
            bool swconnect = false;
            if(c->outputs().size() == 0){
                gibnw = _adg->node(nwid);
                gibsw = _adg->node(swid);
                if(gibnw->inputs().size() < gibsw->inputs().size()){
                    l = getlink(linkid, c, gibnw);
                    _adg->addLink(linkid++, l);
                }
                else{
                    l = getlink(linkid, c, gibsw);
                    _adg->addLink(linkid++, l);
                }
            }
        }
        else{
            neid = cid + Y - 1;
            seid = cid + Y;
            bool neconnect = false;
            bool seconnect = false;
            if(c->outputs().size() == 0){
                gibne = _adg->node(neid);
                gibse = _adg->node(seid);
                if(gibse->inputs().size() < gibne->inputs().size()){
                    l = getlink(linkid, c, gibse);
                    _adg->addLink(linkid++, l);
                }
                else{
                    l = getlink(linkid, c, gibne);
                    _adg->addLink(linkid++, l);
                }
            }
        }
        l = new ADGLink(linkid);
        l->setSrcId(0);
        l->setSrcPortIdx(idx++);
        l->setDstId(cid);
        l->setDstPortIdx(0);
        _adg->addLink(linkid++, l);
    }
    for(const auto& cid : obs){
        c = _adg->node(cid);
        if(cid > middle){
            nwid = cid - rows - 1;
            swid = cid - rows;
            if(c->inputs().size() == 0){
                gibnw = _adg->node(nwid);
                gibsw = _adg->node(swid);
                if(gibnw->outputs().size() < gibsw->outputs().size()){
                    l = getlink(linkid, gibnw, c);
                    _adg->addLink(linkid++, l);
                    //std::cout<< "check ob" << cid << ", gib: " << nwid << std::endl;
                }
                else{
                    l = getlink(linkid, gibsw, c);
                    _adg->addLink(linkid++, l);
                    //std::cout<< "check ob" << cid << ", gib: " << swid << std::endl;
                }
            }
        }
        else{
            neid = cid + rows;
            seid = cid + rows + 1;
            if(c->inputs().size() == 0){
                gibne = _adg->node(neid);
                gibse = _adg->node(seid);
                if(gibse->outputs().size() < gibne->outputs().size()){
                    l = getlink(linkid, gibse, c);
                    _adg->addLink(linkid++, l);
                }
                else{
                    l = getlink(linkid, gibne, c);
                    _adg->addLink(linkid++, l);
                }
            }
        }
        l = new ADGLink(linkid);
        l->setSrcId(cid);
        l->setSrcPortIdx(0);
        l->setDstId(0);
        l->setDstPortIdx(odx++);
        _adg->addLink(linkid++, l);
    }
    for(const auto& cid : pes){
        c = _adg->node(cid);
        nwid = cid + rows;
        swid = cid + rows + 1;
        neid = cid - rows - 1;
        seid = cid - rows;
        gibnw = _adg->node(nwid);
        gibsw = _adg->node(swid);
        gibne = _adg->node(neid);
        gibse = _adg->node(seid);
        bool nwo = false;
        bool swo = false;
        bool neo = false;
        bool seo = false;
        bool nwi = false;
        bool swi = false;
        bool nei = false;
        bool sei = false;
        for(const auto& it: c->outputs()){
            for(const auto& d: it.second){
                if(d.first == nwid){
                    nwo = true;
                }
                if(d.first == swid){
                    swo = true;
                }
                if(d.first == neid){
                    neo = true;
                }
                if(d.first == seid){
                    seo = true;
                }
            }
        }
        for(const auto& it: c->inputs()){
            if(it.second.first == nwid){
                nwi = true;
            }
            if(it.second.first == swid){
                swi = true;
            }
            if(it.second.first == neid){
                nei = true;
            }
            if(it.second.first == seid){
                sei = true;
            }
        }
        if(nwo == false){
            l = getlink(linkid, c, gibnw);
            _adg->addLink(linkid++, l);
        }
        if(swo == false){
            l = getlink(linkid, c, gibsw);
            _adg->addLink(linkid++, l);
        }
        if(neo == false){
            l = getlink(linkid, c, gibne);
            _adg->addLink(linkid++, l);
        }
        if(seo == false){
            l = getlink(linkid, c, gibse);
            _adg->addLink(linkid++, l);
        }
        if(nwi == false){
            l = getlink(linkid, gibnw, c);
            _adg->addLink(linkid++, l);
        }
        if(swi == false){
            l = getlink(linkid, gibsw, c);
            _adg->addLink(linkid++, l);
        }
        if(nei == false){
            l = getlink(linkid, gibne, c);
            _adg->addLink(linkid++, l);
        }
        if(sei == false){
            l = getlink(linkid, gibse, c);
            _adg->addLink(linkid++, l);
        }
    }
    for(const auto& cid: gibs){
        c = _adg->node(cid);
        bool innerconnected = false;
        for(const auto& ot: c->outputs()){
            if(c->out2ins(ot.first).size() > 0){
                innerconnected = true;
                break;
            }
        }
        if(innerconnected == false){
            for(const auto& ot: c->outputs()){
                for(const auto& it: c->inputs()){
                    c->addOut2ins(ot.first, it.first);
                }
            }
        }
    }
    }

    //set ADG node parameters
    for(auto& it : _adg->nodes()){
        ADGNode* node = it.second;
        node->setBitWidth(bitWidth);
        node->setCfgBlkIdx(node->id());
        int numInputs = node->inputs().size() > 0 ? node->inputs().size() : 1;
        int numOutputs = node->outputs().size() > 0 ? node->outputs().size() : 1;
        node->setNumInputs(numInputs);
        node->setNumOutputs(numOutputs);
        if(node->type() == "GPE"){
            if(node->operations().empty()) { node->addOperation("PASS"); }
            int maxNumOperands = 0;
            int numReg = 1;
            int delay = PEDelay;
            for(const auto& operation : node->operations()){
                int numOperands = Operations::numOperands(operation);
                int latency = Operations::latency(operation);
                if(numOperands > maxNumOperands) { maxNumOperands = numOperands; }
                //if(latency > delay) { delay = latency; }
            }
            node->setNumOperands(maxNumOperands);
            node->setNumRfReg(numReg);
            node->setMaxDelay(delay);
        }
        else if(node->type() == "GIB"){
            node->setTrackReged(true);
        }
        node->setSubADG(generateSubADG(node));
    }
    //set ADG parameters
    _adg->setBitWidth(bitWidth);
    _adg->setCfgDataWidth(cfgDataWidth);
    _adg->setCfgAddrWidth(cfgAddrWidth);
    _adg->setCfgBlkOffset(cfgBlkOffset);
    _adg->setNumInputs(numIB);
    _adg->setNumOutputs(numOB);
    _adg->setNumGpeNodes(numPE);
}

// execute ADG generation
bool Generator::execute(int data_width, int cfg_data_width, int cfg_addr_width, int cfg_blk_offset, int pe_delay, std::string type){
    //initialize CGRA ADG parameters
    arch_type = type;

    analyzescale();

    //generate ADG links
    int id = 0;
    int idx = 0;
    for(const auto& dfg : _dfgs){
        int numNode = dfg->nodes().size();
        std::map<int, int> nodemap; // <DFG node id, ADG node id>
        std::map<int, int> imap;
        std::map<int, int> omap;
        std::set<int> penodes = pes;
        std::set<int> ibnodes = ibs;
        std::set<int> obnodes = obs;
        std::set<int> empty;
        std::set<int> dfgnodes;
        std::set<int> usedlinks;
        std::set<int> usedswitches;
        std::map<int, std::pair<int, std::string>> usednodes; // <ADG node id, <DFG node id, DFG node name>>
        for(const auto& i : dfg->nodes()){
            dfgnodes.emplace(i.second->id());
        }
        
        placeins(dfg, imap, nodemap, dfgnodes, ibnodes, penodes);
        placeouts(dfg, omap, nodemap, dfgnodes, obnodes, penodes);
        while(!dfgnodes.empty()){
            int nodeid = *dfgnodes.begin();
            placenode(nodeid, dfg, dfgnodes, nodemap, empty, penodes, obnodes);
        }
        std::cout << "place dfg complete, start to route." << std::endl;

        // route dfg edges
        for(const auto& it : dfg->edges()){
            DFGEdge* edge = it.second;
            int srcId, dstId, srcPortIdx, dstPortIdx;
            int src = it.second->srcId();
            int dst = it.second->dstId();
            int srcPort = it.second->srcPortIdx();
            int dstPort = it.second->dstPortIdx();
            if(src == 0){
                srcId = imap[srcPort];
                srcPortIdx = 0;
            }
            else{
                srcId = nodemap[src];
                srcPortIdx = srcPort;
            }
            if(dst == 0){
                dstId = omap[dstPort];
            }
            else{
                dstId = nodemap[dst];
            }
            bool found = findconnection(srcId, srcPortIdx, dstId, dstPortIdx, usedlinks, usedswitches);
            if(found == false){
                std::cout << src << ", " << srcPort << " src id: " << srcId << ", " << srcPortIdx << ". " << dst << ", " << dstPort << ", dst id: " << dstId << ", " << dstPortIdx << ", link id: " << id << std::endl;
                addconnection(id, srcId, srcPortIdx, dstId, dstPortIdx, usedlinks, usedswitches);
            }
            else{ std::cout << "connection found." << std::endl; }
        }
        std::cout << "route complete." << std::endl;

        //record netlist information
        for(const auto& it : imap){
            usedswitches.emplace(it.second);
        }
        for(const auto& it : omap){
            usedswitches.emplace(it.second);
        }
        for(const auto& it : nodemap){
            usednodes[it.second] = std::make_pair(it.first, dfg->node(it.first)->name());
        }
        _nodemaps.emplace_back(nodemap);
        _imaps.emplace_back(imap);
        _omaps.emplace_back(omap);
        _linksets.emplace_back(usedlinks);
        _switchsets.emplace_back(usedswitches);
        _nodesets.emplace_back(usednodes);
    }

    formalize(data_width, cfg_data_width, cfg_addr_width, cfg_blk_offset, pe_delay);
    return true;
}

// get max latency of the mapped DFG
int Generator::getMaxLat()
{
    int maxLat = 0;
    for(const auto& mapping : _mappings){
        if(maxLat < mapping->maxLat()){
            maxLat = mapping->maxLat();
        }
    }
    return maxLat;
}
