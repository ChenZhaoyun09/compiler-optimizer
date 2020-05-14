#include <cstdio>
#include <vector>
#include <queue>
using namespace std;

struct Refer_unit {
	char name[16];
	char func_unit[16];
	int cycle;
	char input[3][16];
	char output[2][16];
	int r_cycle, w_cycle;

	void clear() {
		memset(name, 0, sizeof(name));
		memset(func_unit, 0, sizeof(func_unit));
		memset(input, 0, sizeof(input));
		memset(output, 0, sizeof(output));
	}
};

struct Refer_table {
	vector<Refer_unit> table;
	
	void load_table(char* filename) {
		int i,ii;
		char instr_info[256],buf[16];
		Refer_unit x;
		FILE* fp = fopen(filename, "r");

		fscanf(fp, "%s", instr_info);
		while (fscanf(fp, "%s", instr_info) != EOF) {
			x.clear();
			for (ii = i = 0; instr_info[i] != ','; i++, ii++)
				x.name[ii] = instr_info[i];
			for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
				x.func_unit[ii] = instr_info[i];
			for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
				buf[ii] = instr_info[i];
			buf[ii] = '\0';
			x.cycle = atoi(buf);
			for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
				x.input[0][ii] = instr_info[i];
			for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
				x.input[1][ii] = instr_info[i];
			for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
				x.input[1][ii] = instr_info[i];
			for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
				x.input[2][ii] = instr_info[i];
			for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
				x.output[0][ii] = instr_info[i];
			for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
				x.output[1][ii] = instr_info[i];
			for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
				buf[ii] = instr_info[i];
			buf[ii] = '\0';
			x.r_cycle = atoi(buf);
			for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
				buf[ii] = instr_info[i];
			buf[ii] = '\0';
			x.w_cycle = atoi(buf);
			table.push_back(x);
		}
	}
};

struct Instr {
	int indeg;
	char* instr_str;
	int end_time;
	char* func_unit;
	vector<Instr*> fa;
	vector<Instr*> chl;

	bool operator < (Instr& x) {
		return indeg > x.indeg;
	}
};

struct Topograph {
	priority_queue<Instr> nodes;

	void clear() {
		while (!nodes.empty()) {
			free((nodes.top()).instr_str);
			free((nodes.top()).func_unit);
			nodes.pop();
		}
	}

	inline void push(Instr x) {
		nodes.push(x);
	}
};

static int first_word_length(char* str) {
	int i,ii;
	for (i = 0; str[i] == '\t' || str[i] == '|'; i++);
	for (ii = i; str[ii] != '\t' && str[ii] != '\n'; ii++);
	return ii - i;
}

//void build_topograph(FILE *fp, FILE *refer) {
//	Instr one;
//	char buf[64];
//	int i,ii;
//	while (fscanf(fp, "%s", one.instr_str)) {
//		for (i = 0; one.instr_str[i] == '\t' || one.instr_str[i] == '|'; i++);
//		for (ii = 0; one.instr_str[i] != '\t' && one.instr_str[i] != '\n'; i++, ii++)
//			buf[ii] = one.instr_str[i];
//		if(buf[0]=='.' || )
//		printf("%s", one.instr_str);
//	}
//}