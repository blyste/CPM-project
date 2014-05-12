#ifndef DESC_H
#define DESC_H

#include "components.h"
#include "Cpm.h"
#include <map>
#include <string>
#include <list>
#include <vector>

//Dependency of one wire to its nearest parents
//=Gate's output to its direct inputs
extern std::map<std::string, std::list<std::string> > wire_deps_direct;
extern std::map<std::string, std::list<std::string> > wire_deps;

//Which wire is used for which
//=One wire to gate's output 
extern std::map<std::string, std::list<std::string> > wire_used;

typedef enum {
	Unknown,		//Not processed yet
	UnknownTerminal,	//A module input
	Unique,			//Single usage intermediate
	UniqueTerminal,		//A module input used only once
	Node,			//Multiple usage intermediate
	NodeTerminal,		//Multiple usage module input
	Output,			//Module output
} WireType;


extern std::map<std::string, WireType> wire_type;

//List of known gates
extern std::map<std::string, GateType> gate_types;

extern std::map<std::string, CPM> wire_cpm;

//Helpers
static inline bool wireIsNode(WireType t) {
	return t == Node || t==NodeTerminal;
}

static inline bool wireIsUnique(WireType t) {
	return t == Unique || t==UniqueTerminal;
}

static inline bool wireIsNode(std::string t) {
	return wireIsNode(wire_type[t]);
}

static inline bool wireIsUnique(std::string t) {
	return wireIsUnique(wire_type[t]);
}

#endif /* DESC_H */
