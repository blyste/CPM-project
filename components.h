#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <list>
#include <string>
#include <vector>
#include "Cpm.h"

class GateType {
	public:
		GateType() {};
		GateType(const std::list<std::string>& args);
		void parse(std::list<std::string>& args);

		CPM operator()(std::string A, std::string B, std::string dst);
	private:
		std::vector<int> truth;
		int n_inputs;

};
extern void parse_components();

#endif /* COMPONENTS_H */
