#include "Desc.h"

std::map<std::string, std::list<std::string> > wire_deps_direct;
std::map<std::string, std::list<std::string> > wire_deps;
std::map<std::string, std::list<std::string> > wire_used;
std::map<std::string, WireType> wire_type;
std::map<std::string, GateType> gate_types;
std::map<std::string, CPM> wire_cpm;
