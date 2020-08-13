#pragma once
#include <cstdio>
#include <vector>
#include <cstring>
#include <queue>
#include "analyser.h"
#include "schedule.h"
using namespace std;


inline int peek(FILE* fp) {
	int c = fgetc(fp);
	ungetc(c, fp);
	return c;
}

inline long long stoll(char* str) {
	if (str[0] != '0') return atoll(str);
	else {
		long long res;
		sscanf(str, "%llx", &res);
		return res;
	}
}

inline bool mem_rely(const mem_info& fa, const mem_info& chl) {
	if (isalpha(chl.OR[0]) || isalpha(fa.OR[0])) return true;
	else if (!strcmp(fa.AR, chl.AR)) return true;
	else if (fa.AR_offset == chl.AR_offset) return true;
	else return false;
}

inline void add_num_after_str(char* destination, char* source, int num) {
	strcpy(destination, source);
	sprintf(destination + strlen(destination), "%d", num);
}

bool reg_equal(char* str_conj, char* str);

void process_file(char* destination, char* source, void (Topograph::* Func)(FILE* fp));