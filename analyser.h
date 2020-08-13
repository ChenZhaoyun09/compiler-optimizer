#pragma once
#include <cstdio>
#include <vector>
#include <cstring>
#include <queue>
#include <unordered_map>
#include <map>
using namespace std;

const int INSTR_MAX_SIZE = 512;

int findchar(char* source, char c);
int hash_str(const char* str);
int get_first_word(char* destination, char* source);
//0: instruction;	1: label;	3: others;
int sentence_type(char* str, char* sentence);


struct Refer_unit {
	char name[16];
	char func_unit[16];
	int cycle;
	char input[3][16];
	char output[1][16];
	int r_cycle, w_cycle;
};

struct Refer_table {
	vector<Refer_unit> table;

	void load_table(char* filename);
	Refer_unit get_refer_unit(char* str);
	
};

struct Instr {
	char* instr_str;
	char instr_name[16];
	char func_unit[16];
	char cond[16];
	char input1[16], input2[16], input3[16];
	char output1[16];
	int cycle, r_cycle, w_cycle;
	int indeg, last_fa_end_time;
	vector<pair<Instr*, int> > chl;

	Instr() {
		func_unit[0] = '\0';
		cond[0] = '\0';
		input1[0] = '\0'; input2[0] = '\0'; input3[0] = '\0';
		output1[0] = '\0';
		cycle = r_cycle = w_cycle = 0;
		indeg = last_fa_end_time = 0;
	}

	void init() {
		cond[0] = '\0';
		input1[0] = '\0'; input2[0] = '\0'; input3[0] = '\0';
		output1[0] = '\0';
		cycle = r_cycle = w_cycle = 0;
		indeg = last_fa_end_time = 0;
		chl.resize(0);
	}
};

struct cd_Instr {
	Instr* instr_p;
	int counter;
	long long AR_offset;

	cd_Instr(Instr* x = NULL, int cnt = 0, long long offset = 0) {
		instr_p = x;
		counter = cnt;
		AR_offset = offset;
	}

	bool operator < (const cd_Instr& b)const {
		return counter > b.counter;
	}
};

struct reg_info {
	char name[16];

	reg_info(char* x = NULL) {
		if (x != NULL) strcpy(name, x);
		else name[0] = '\0';
	}

	bool operator == (const reg_info& x)const {
		return !strcmp(name, x.name);
	}
};
struct reg_info_cmp {
	int operator () (const reg_info& x)const {
		return hash_str(x.name);
	}
};

struct mem_info {
	char AR[8];
	char OR[16];
	long long AR_offset;

	mem_info(char* AR1 = NULL, char* OR1 = NULL, long long x = 0) {
		if (AR1 != NULL) strcpy(AR, AR1);
		else AR[0] = '\0';
		if (OR1 != NULL) strcpy(OR, OR1);
		else OR[0] = '\0';
		AR_offset = x;
	}
};
struct mem_info_cmp {
	bool operator() (const mem_info& a, const mem_info& b)const {
		int res = strcmp(a.AR, b.AR);
		if (res) return res < 0;
		res = strcmp(a.OR, b.OR);
		if (res) return res < 0;
		return a.AR_offset < b.AR_offset;
	}
};


struct Topograph {
	int Timer, instr_num;
	Instr All_Instr[INSTR_MAX_SIZE]; // store all instr
	vector<Instr*> container; // store pointers of all Instr
	priority_queue<cd_Instr> cd_buff;
	unordered_map<reg_info, Instr*, reg_info_cmp> reg_written, up_AR_change;// unpredictable AR change
	map<mem_info, Instr*, mem_info_cmp> mem_written, mem_read;
	unordered_map<reg_info, long long, reg_info_cmp> reg_offset;


	Topograph() {
		Timer = 0;
		instr_num = 0;
	}

	void clear() {
		int len = container.size();
		for (int i = 0; i < len; i++) {
			free(container[i]->instr_str);
		}
	}

	Instr* generate_Instr(char* sentence);

	// function below are defined in 'Topo_relative.h'
	void reschedule(FILE* fp);
	void push_to_cd_buff(Instr* x);
	void fresh_cd_buff();
	void build_read_reg_dependency(Instr* x, char* input);
	void build_read_mem_dependency(Instr* x, char* input, int load_store);
	//void build_dependency(Instr* x);
	void output_topo_graph(FILE* fp);
};