#include "tools.h"
#include "Desc.h"
#include "components.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <string>
#include <sstream>
#include <string.h>
#include <stdlib.h>

void parse_syn(const std::string& filename="c17.v") {
	std::string line;
	std::ifstream file(filename);

	if(!file.is_open())
		throw "bite";

	while ( getline (file,line) ) {
		auto t = split(line, " ,;()\r\n");
		if(t.size()==0)
			continue;

		std::string op = t.front();
		t.pop_front();
		if(op=="input") {
			for(const std::string& elem: t) {
				wire_type[elem] = UnknownTerminal;
			}
		} else if(op=="output") {
			for(const std::string& elem: t) {
				wire_type[elem] = Output;
			}
		} else if(op=="wire") {
			for(const std::string& elem: t) {
				wire_type[elem] = Unknown;
			}
		} else if(gate_types.find(op) != gate_types.end()) {
			//Ignore module name
			t.pop_front();

			gate_types[op].parse(t);
		}
	}
	file.close();
}

void compute_types() {
	for(auto &wire: wire_type) {
		if(wire.second != Unknown && wire.second != UnknownTerminal) 
			continue;
		if(wire_used[wire.first].size() <=1 )
			wire.second = (wire.second == Unknown) ? Unique : UniqueTerminal;
		else
			wire.second = (wire.second == Unknown) ? Node : NodeTerminal;
	}
}

void add_child(const std::string& origin, const std::string& parent) {
	if(wire_type[parent] == Node ||
			wire_type[parent] == UniqueTerminal ||
			wire_type[parent] == NodeTerminal) {
		wire_deps[origin].push_front(parent);
	} else {
		for(const auto& p: wire_deps_direct[parent])
			add_child(origin, p);
	}
}

void compute_usage() {
	for(auto wire: wire_deps_direct) {
		std::string name(wire.first);
		for(auto parent: wire.second) {
			add_child(name, parent);
		}
	}
}

CPM compute_cpm(const std::string& wire) {
	//If it's not a node, then the computation is simple:
	const auto& deps = wire_deps_direct[wire];
	if(deps.size() != 2) {
		std::cerr << "Too lazy to handle more than two inputs ! " << deps.size() << wire << std::endl;
		exit(1);
	}

	return gate_types["nand"](deps.front(), deps.back(), wire);
}

void resolve_stack(std::stack<std::string>& s) {
	while(s.size()>0) {
		std::string wire = s.top();
		//Already computed
		if(wire_cpm.find(wire) != wire_cpm.end()) {
			s.pop();
			continue;
		}

		bool can_compute = true;
		for(const auto& d: wire_deps_direct[wire]) {
			if(wire_cpm.find(d) == wire_cpm.end()) {
				std::cout << wire << " requires " << d << std::endl;
				can_compute = false;
				s.push(d);
			}
		}
		if(can_compute) {
			std::cout << "Computing " << wire << std::endl;
			wire_cpm[wire]=compute_cpm(wire);
			s.pop();
		}
	}
}

double get_element(std::string wire, std::map<std::string,int> iterators) {
	int i = 0;
	int m_idx = 0;
	for(const auto& w: wire_cpm[wire].deps) {
		m_idx += iterators[w] << (2*i);
		++i;
	}
	return wire_cpm[wire].probs[m_idx][iterators[wire]];
}

double final_compute(std::string wire, int pos = -1) {
	if(pos == -1) {
		return
//Used to check sum of probs = 1
#if 0
			final_compute(wire, 1)
			+ final_compute(wire, 2)+
#endif
			final_compute(wire, 0)
			+ final_compute(wire, 3);
	}
	//Construct the list of deps
	std::vector<std::string> nodes;
	for(const auto &w: wire_type) {
		if(wireIsNode(w.second)) {
			nodes.push_back(w.first);
		}
	}

	double prob = 0;
	for(unsigned int i=0; i<(1<<(2*nodes.size())); ++i) {
		std::map<std::string, int> iterators;
		int k = 0;
		for(const auto& w: nodes) {
			iterators[w]=(i>>(2*k))&3;
			++k;
		}
		iterators[wire] = pos;

		double p = 1;
		for(auto n: nodes) {
			p *= get_element(n, iterators);
		}
		p *= get_element(wire, iterators);

		prob += p;
	}
	return prob;
}

int main(void) {
	parse_components();
	//parse_syn("exemple_article.v");
	parse_syn("exemple_article.v");
	compute_types();
	compute_usage();
	for(auto wire: wire_deps_direct) {
		std::cout << wire.first << " directly depends on " << std::endl;
		for(auto dep: wire.second) {
			std::cout << "- " << dep << std::endl;
		}
	}
	std::cout << std::endl;

	for(auto wire: wire_deps) {
		std::cout << wire.first << " depends on " << std::endl;
		for(auto dep: wire.second) {
			std::cout << "- " << dep << std::endl;
		}
	}
	std::cout << std::endl;

	//Put all outputs on the stack
	std::stack<std::string> wire_stack;
	for(const auto &w: wire_type) {
		if(w.second == Output) {
			wire_stack.push(w.first);
		}
	}

	std::vector<double> input_cpm;
	input_cpm.push_back(0.5);
	input_cpm.push_back(0);
	input_cpm.push_back(0);
	input_cpm.push_back(0.5);

	//Set all inputs as {0.5, 0, 0, 0.5}
	for(const auto &w: wire_type) {
		if(w.second == UniqueTerminal || w.second == NodeTerminal) {
			wire_cpm[w.first] = CPM(input_cpm);
		}
	}

	//And now, resolve it
	resolve_stack(wire_stack);

	std::cout << std::endl;
	std::cout << std::endl;
	for(const auto &w: wire_type) {
		if(w.second == Output) {
			double res = final_compute(w.first);
			std::cout << w.first << ":" <<
				res
				<< std::endl;
		}
	}

	return 0;
}
