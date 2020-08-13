#pragma once
#include <cstdio>
#include <cstring>
#include <queue>
#include <unordered_map>
#include "analyser.h"

using namespace std;

const int num_of_func_unit = 11;
const int max_num_of_instr = 1024;

inline bool isalpha(char x) {
	return ('a' <= x && x <= 'z') || ('A' <= x && x <= 'Z');
}
inline bool isdigit(char x) {
	return '0' <= x && x <= '9' || x == '-';
}
inline bool isaord(char x) {
	return isalpha(x) || isdigit(x);
}

inline bool skip_char(char c) {
	return c == '\t' || c == '|' || c == ' ' || c == '\n';
}

struct func_name {
	char name[16];
	func_name(const char* x) {
		strcpy(name, x);
	}
};

struct func_name_cmp {
	bool operator () (const func_name& x, const func_name& y)const {
		return strcmp(x.name, y.name);
	}
};
struct Instr_cmp1 {
	bool operator() (const Instr* x, const Instr* y) const {
		return x->cycle < y->cycle;// Instr in high priority with long cycle
	}
};

void load_func_table();

static Instr* schedule_queue_r[num_of_func_unit][max_num_of_instr];
static Instr* schedule_queue_w[num_of_func_unit][max_num_of_instr];
static priority_queue<Instr*, vector<Instr*>, Instr_cmp1> zero_indeg_instr;
static int sq_max_len = 0;


static pair<int, int> find_fit(pair<int, int> pr, int start, int cycle, int read_cycle, int write_cycle);

static void place(Instr* x, int no_func, int time_pos);

static void unplace(Instr* x, int no_func, int time_pos);

static void realloc_mac(Instr* x, int no_func);

static void load_zero_indeg_instr(Topograph* topo);

static void output_reschedule_result(FILE* fp);

static bool Instr_cmp(const Instr* x, const Instr* y);