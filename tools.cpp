#include "tools.h"
#include <string.h>
#include <stdlib.h>

std::list<std::string> split(std::string str, const char* delim) {
	std::list<std::string> result;
	char *to_be_freed = strdup(str.c_str());
	char *tmp = to_be_freed;
	while(*tmp != 0) {
		//Ignore delimiters
		size_t res = strspn(tmp, delim);
		tmp += res;

		if(*tmp == 0)
			break;
		res = strcspn(tmp, delim);
		if(res == 0)
			break;
		result.push_back(std::string(tmp, res));
		tmp+=res;
	}
	if(to_be_freed)
		free(to_be_freed);
	return result;
}

int log2_bin(int v) {
	int r = 0; // r will be lg(v)

	while (v >>= 1) // unroll for more speed...
	{
		r++;
	}
	return r;
}
