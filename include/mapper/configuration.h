#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "mapper/mapping.h"



// configuration data
struct CfgData{
    int len; // config data length
    std::vector<uint32_t> data; // config data
    CfgData(){}
    CfgData(int len_) : len(len_){}
    CfgData(int len_, uint32_t data_){
        len = len_;
        data.push_back(data_);
    }
    CfgData& operator=(const CfgData& that){
        if(this == &that) return *this;
        len = that.len;
        data = that.data;
        return *this;
    }
};


// configuration data packet
struct CfgDataPacket{
    unsigned addr; // config address
    std::vector<uint32_t> data; // config data
    CfgDataPacket(unsigned addr_) : addr(addr_){}
    CfgDataPacket(unsigned addr_, uint32_t data_){
        addr = addr_;
        data.push_back(data_);
    }
};


// CGRA Configuration
class Configuration
{
private:
    Mapping* _mapping;
public:
    Configuration(Mapping* mapping) : _mapping(mapping) {}
    ~Configuration(){}
    // get config data for GPE, return<LSB-location, CfgData>
    std::map<int, CfgData> getGpeCfgData(ADGNode* node);
    // get config data for GIB and IOB, return<LSB-location, CfgData>
    std::map<int, CfgData> getGibIobCfgData(ADGNode* node);
    // get config data for ADG node
    void getNodeCfgData(ADGNode* node, std::vector<CfgDataPacket>& cfg);
    // get config data for ADG
    void getCfgData(std::vector<CfgDataPacket>& cfg);
    // dump config data
    void dumpCfgData(std::ostream& os);
};






#endif