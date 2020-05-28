#pragma once
#include <cstdio>
#include <cstring>
#include <queue>
#include <unordered_map>
#include "analyser.h"
using namespace std;

const int num_of_func_unit = 11;
const int max_num_of_instr = 1024;


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

unordered_map<string, pair<int, int> > func_no;

void load_func_table() {
	func_no["M1/M2"] = make_pair(0, 1);
	func_no["SIEU"] = make_pair(2, 2);
	func_no["SLDST"] = make_pair(3, 3);
	func_no["SBR"] = make_pair(4, 4);
	func_no["M1/M2/M3"] = make_pair(5, 7);
	func_no["VIEU"] = make_pair(8, 8);
	func_no["VLDST0"] = make_pair(9, 9);
	func_no["VLDST1"] = make_pair(10, 10);
}



static Instr* schedule_queue_r[num_of_func_unit][max_num_of_instr];
static Instr* schedule_queue_w[num_of_func_unit][max_num_of_instr];
static queue<Instr*> zero_indeg_instr;
static int sq_max_len = 0;



static pair<int, int> find_fit(char* func_unit, int start, int cycle, int read_cycle, int write_cycle) {
	//make sure start >= 0
	if (start < 0) start = 0;

	pair<int, int> pr = func_no[string(func_unit)];
	int best_fit = 0x3f3f3f3f, no_func;
	int i, j, k;
	bool flag;
	for (i = pr.first; i <= pr.second; i++) {
		for (j = start; j < max_num_of_instr; j++) {
			flag = 1;
			for (k = j; k < j + read_cycle; k++) {
				if (schedule_queue_r[i][k] != NULL) {
					flag = 0;
					break;
				}
			}
			if (flag == 0) { j = k; continue; }
			for (k = j + cycle - write_cycle; k < j + cycle; k++) {
				if (schedule_queue_w[i][k] != NULL) {
					flag = 0;
					break;
				}
			}
			if (flag == 0) { j += read_cycle - 1; continue; }
			else if (j < best_fit) {
				no_func = i;
				best_fit = j;
				break;
			}
		}
	}
	if (best_fit == 0x3f3f3f3f) {
		printf("schedule_queue is full, fail to find a fit.\ntry modifying max_num_of_instr in \'schedule.h\' to a larger size\n");
		exit(1);
	}
	return make_pair(no_func, best_fit);
}

static void place(Instr* x, int no_func, int time_pos) {
	Instr* child; int edge_len;

	//place this instruction into schedule_queue
	for (int i = time_pos; i < time_pos + x->r_cycle; i++)
		schedule_queue_r[no_func][time_pos] = x;
	for (int i = time_pos + (x->cycle) - (x->w_cycle); i < time_pos + x->cycle; i++)
		schedule_queue_w[no_func][time_pos] = x;

	if (time_pos + (x->cycle) > sq_max_len) sq_max_len = time_pos + (x->cycle);

	//fresh children's information
	for (int i = 0; i < (x->chl).size(); i++) {
		child = (x->chl[i]).first;
		edge_len = (x->chl[i]).second;// be careful: edge_len != x->cycle !!!!
		if (child->last_fa_end_time < time_pos + edge_len) child->last_fa_end_time = time_pos + edge_len;
		(child->indeg)--;
		if (child->indeg == 0) zero_indeg_instr.push(child);
	}
}

static void unplace(Instr* x, int no_func, int time_pos) {
	for (int i = time_pos; i < time_pos + x->r_cycle; i++)
		schedule_queue_r[no_func][time_pos] = NULL;
	for (int i = time_pos + (x->cycle) - (x->w_cycle); i < time_pos + x->cycle; i++)
		schedule_queue_w[no_func][time_pos] = NULL;
}

static void realloc_mac(Instr* x, int no_func) {
	int pos = findchar(x->instr_str, '.');
	int no_mac = no_func - func_no[string(x->func_unit)].first + 1;
	x->instr_str[pos + 2] = no_mac + '0';
}

static void load_zero_indeg_instr(Topograph* topo) {
	// reset sq_max_len
	sq_max_len = 0;
	// clear queue
	while (zero_indeg_instr.size()) zero_indeg_instr.pop();

	for (int i = 0; i < (topo->container).size(); i++) {
		if ((topo->container)[i]->indeg == 0) {
			zero_indeg_instr.push(topo->container[i]);
		}
	}
}

static void output_reschedule_result(FILE* fp) {
	Instr* x;
	bool SNOP, first_instr;
	int snop = 0, pos;

	for (int j = 0; j < sq_max_len; j++) {
		SNOP = 1;
		first_instr = 1;
		for (int i = 0; i < num_of_func_unit; i++) {
			if (schedule_queue_r[i][j] == NULL) continue;

			SNOP = 0;
			x = schedule_queue_r[i][j];
			unplace(x, i, j);
			for (pos = 0; skip_char(x->instr_str[pos]); pos++);

			if (first_instr) {
				first_instr = 0;
				if (snop) {
					fprintf(fp, "\t\tSNOP\t\t%d\n", snop);
					snop = 0;
				}
				fprintf(fp, "\t\t%s", x->instr_str + pos);
			}
			else fprintf(fp, "\t|\t%s", x->instr_str + pos);

		}
		if (SNOP) snop++;
	}
}


void Topograph::reschedule(FILE* fp) {
	Instr* x, * sbr = NULL;
	int pos;
	pair<int, int> pr;

	load_zero_indeg_instr(this);

	while (zero_indeg_instr.size()) {
		x = zero_indeg_instr.front();
		zero_indeg_instr.pop();

		// if this instruction is SNOP
		if ((pos = findchar(x->instr_str, 'S')) != -1) {
			if (!strncmp(x->instr_str + pos, "SNOP", 4))
				continue;
			if (!strncmp(x->instr_str + pos, "SBR", 3)) {
				sbr = x;
				continue;
			}
		}

		pr = find_fit(x->func_unit, x->last_fa_end_time, x->cycle, x->r_cycle, x->w_cycle);
		if (!strncmp(x->func_unit, "M1/M2", 5)) realloc_mac(x, pr.first);
		place(x, pr.first, pr.second);
	}

	if (sbr != NULL) {
		pr = find_fit(sbr->func_unit, max(sbr->last_fa_end_time, sq_max_len - (sbr->cycle)), sbr->cycle, sbr->r_cycle, sbr->w_cycle);
		place(sbr, pr.first, pr.second);
	}

	output_reschedule_result(fp);
}



static bool Instr_cmp(const Instr* x, const Instr* y) {
	return x->indeg < y->indeg;
}


void Topograph::output_topo_graph(FILE* fp) {
	Instr* x;
	int no;

	//printf("reg_written info:\n");
	//for (unordered_map<reg_info, Instr*, reg_info_cmp>::iterator it = this->reg_written.begin(); it != this->reg_written.end(); it++) {
	//	x = it->second;
	//	printf("%s", x->instr_str);
	//}puts("");

	sort(this->container.begin(), this->container.end(), Instr_cmp);
	for (int i = 0, len = this->container.size(); i < len; i++) {
		x = this->container[i];
		// fprintf(fp, "%schildren: ", x->instr_str);
		fprintf(fp, "%d:%schildren: ", i, x->instr_str);
		for (int ii = 0, len2 = x->chl.size(); ii < len2; ii++) {
			no = find(this->container.begin(), this->container.end(), (x->chl[ii]).first) - this->container.begin();
			// fprintf(fp, "%d ", no);
			fprintf(fp, "(%d, %d) ", no, x->chl[ii].second);
		}// fprintf(fp, "\n");
		fprintf(fp, "\n");
	}
	// fprintf(fp, "\n");
	fprintf(fp, "\n");
}
