#ifndef __NETLIST_H__
#define __NETLIST_H__

#include "mapper/mapping.h"

class Netlist
{
private:
	std::set<int> _nets;
	std::map<int, std::pair<int, std::string>> _nodes;
	std::set<int> _switches;
public:
	Netlist()	{}
	Netlist(Mapping* mapping);
	Netlist(const std::map<int, std::pair<int, std::string>>& nodes, const std::set<int>& nets, const std::set<int>& switches) { _nodes = nodes; _nets = nets; _switches = switches; }
	Netlist(std::string filename) { load(filename); }
	~Netlist()	{}

	void load(std::string );

	std::set<int> nets() { return _nets; }

	std::map<int, std::pair<int, std::string>> nodes() { return _nodes; }
	std::pair<int, std::string> node(int i) { return _nodes[i]; }

	std::set<int> switches() { return _switches; }

	void saveNetlist(std::ostream &os, ADG* adg);
};

#endif