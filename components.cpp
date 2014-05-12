#include "tools.h"
#include "Desc.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

GateType::GateType(const std::list<std::string>& _args) {
	std::list<int> args;
	for(std::string str: _args) {
		int i;
		std::istringstream(str) >> i;
		args.push_front(i);
	}
	n_inputs = log2_bin(args.back());
	args.pop_back();

	truth.resize(1<<n_inputs);
	for(int i = 0; i<(1<<n_inputs); ++i) {
		truth[i] = args.back();
		args.pop_back();
	}
}

void GateType::parse(std::list<std::string>& args) {
	std::string out = args.front();
	args.pop_front();

	for(int i = 0; i<n_inputs; ++i) {
		std::string b = args.front();
		args.pop_front();

		wire_used[b].push_front(out);
		wire_deps_direct[out].push_front(b);
	}
}

void parse_components() {
	std::string line;
	std::ifstream file("components.dat");

	if(!file.is_open())
		throw "bite";

	while ( getline (file,line) ) {
		auto t = split(line, " ;");
		if(t.size() == 0)
			continue;
		std::string type = t.front();
		t.pop_front();

		if(type != "component")
			continue;

		//Component id... don't care
		t.pop_front();

		std::string name = t.front();
		getline(file, line);
		t = split(line, " ;");
		GateType g(t);
		gate_types[name] = g;
	}
}

//ptm = first column of PTM matrix = Probabilities of output = 0
Column computeNew(Column a, Column b, double *ptm) {
	double a0[4];
	double a1[4];

	for(int line_col0 = 0; line_col0<4; line_col0++) {
		a0[line_col0] = 0;
		a1[line_col0] = 0;
		for(int ptm_idx = 0; ptm_idx<4; ptm_idx++) {
			double coef = a[ (line_col0&2) + (ptm_idx/2)] * b[ (line_col0&1)*2 + (ptm_idx&1) ];
			a0[line_col0] += ptm[ptm_idx]	  * coef;
			a1[line_col0] += (1-ptm[ptm_idx]) * coef;
		}
	}

	Column res;
	for(int i = 0; i<4; ++i) {
		//Probabilité d'avoir un 0 > à 1/2
		if(ptm[i] > 0.5) {
			// ITM[i] = 0
			res[_0VRAI] += a0[i];
			res[_1FAUX] += a1[i];
		} else {
			// ITM[i] = 1
			res[_1VRAI] += a1[i];
			res[_0FAUX] += a0[i];
		}
	}

	return res;
}

Column selectColumn(std::string wire, int idx) {
	if(wireIsNode(wire)) {
		Column res;
		res[0] = (idx == 0);
		res[1] = (idx == 1);
		res[2] = (idx == 2);
		res[3] = (idx == 3);
		return res;
	} else {
		return wire_cpm[wire].probs[idx];
	}
}

Column selectColumn(std::string wire, std::map<std::string,int> idx) {
	if(wireIsNode(wire)) {
		Column res;
		res[0] = (idx[wire] == 0);
		res[1] = (idx[wire] == 1);
		res[2] = (idx[wire] == 2);
		res[3] = (idx[wire] == 3);
		return res;
	} else {
		int i = 0;
		int m_idx = 0;
		for(const auto& w: wire_cpm[wire].deps) {
			m_idx += idx[w] << (2*i);
			++i;
		}
		return wire_cpm[wire].probs[m_idx];
	}
}

CPM GateType::operator()(std::string A, std::string B, std::string dst) {
	if(wire_cpm.find(A) == wire_cpm.end()) {
		std::cerr << "Missing source CPM " << A << std::endl;
		exit(1);
	}

	if(wire_cpm.find(B) == wire_cpm.end()) {
		std::cerr << "Missing source CPM " << B << std::endl;
		exit(1);
	}
	CPM ret;

	//double q = 0.9;
	//double q = 0.9894;
	double q = 0.99;
	/*
	 * AND
	 * double ptm[4] = { q, q, q, 1-q };
	 */
	double ptm[4] = { 1-q, 1-q, 1-q, q };

	//Nombre de dépendances dont est reponsable A
	if(wireIsNode(A)) {
		//On ajoute A comme dépendance
		ret(A);
	} else {
		for(const auto& wire: wire_cpm[A].deps) {
			//On ajoute les dépendances de A
			ret(wire);
		}
	}

	if(wireIsNode(B)) {
		//On ajoute B comme dépendances
		ret(B);
	} else {
		for(const auto& wire: wire_cpm[B].deps) {
			//On ajoute les dépendances de B
			ret(wire);
		}
	}
	
	for(unsigned int i = 0; i<ret.probs.size(); ++i) {
		std::map<std::string,int> iterators;
		int k = 0;
		for(const auto& w: ret.deps) {
			iterators[w]=(i>>(2*k))&3;
			++k;
		}

		ret[i] = computeNew(selectColumn(A, iterators),
				selectColumn(B, iterators),
				ptm);
		std::cout << "\t" << dst << "[" << i << "]=" << std::string(ret[i]) << std::endl;
	}

	return ret;
}
