#ifndef CPM_H
#define CPM_H
#include <sstream>

enum {
	_0VRAI = 0,
	_0FAUX = 1,
	_1FAUX = 2,
	_1VRAI = 3
};
class Column {
	public:
		Column() {
			p[0] = 0;
			p[1] = 0;
			p[2] = 0;
			p[3] = 0;
		}
		double& operator[](int v) {
			return p[v];
		}
		double p[4];

		operator std::string() const {
			std::ostringstream oss;
			oss << p[0] << ", ";
			oss << p[1] << ", ";
			oss << p[2] << ", ";
			oss << p[3];
			return oss.str();
		}
};

class CPM {
	public:
		CPM() { probs.resize(1); }
		CPM(const std::vector<double>& a) {
			Column m;
			m[0] = a[0];
			m[1] = a[1];
			m[2] = a[2];
			m[3] = a[3];
			probs.push_back(m);
		}
		CPM(const Column a) {
			probs.push_back(a);
		}
		std::list<std::string> deps;
		std::vector<Column> probs;
		Column& operator[](int v) {
			return probs[v];
		}

		//Append wire to dependencies
		void operator()(std::string wire) {
			for(const auto& w: deps) {
				if(w == wire)
					return;
			}
			deps.push_back(wire);
			//4**N
			probs.resize(1<<(2*deps.size()));
		}

		operator std::string() {
			std::string res;
			res+="Dependencies:\n";
			for(const auto& w: deps) {
				res+="\t"+w+"\n";
			}

			res+="Columns:\n";
			for(const auto& col: probs) {
				res+="\t"+std::string(col)+"\n";
			}
			return res;
		}
};

#endif /* CPM_H */
