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

unordered_map<string, pair<int, int> > func_no;

void load_func_table() {
	func_no["M1/M2"] = make_pair(0, 1);
	func_no["M1/M2/SIEU"] = make_pair(0, 2);
	func_no["SIEU"] = make_pair(2, 2);
	func_no["SLDST"] = make_pair(3, 3);
	func_no["SBR"] = make_pair(4, 4);
	func_no["M1/M2/M3"] = make_pair(5, 7);
	func_no["M1/M2/M3/VIEU"] = make_pair(5, 8);
	func_no["M1"] = make_pair(5, 5);
	func_no["VIEU"] = make_pair(8, 8);
	func_no["VLDST"] = make_pair(9, 10);
}

struct Instr_cmp1 {
	bool operator() (const Instr* x, const Instr* y) const {
		return x->cycle < y->cycle;// Instr in high priority with long cycle
	}
};

static Instr* schedule_queue_r[num_of_func_unit][max_num_of_instr];
static Instr* schedule_queue_w[num_of_func_unit][max_num_of_instr];
static priority_queue<Instr*, vector<Instr*>, Instr_cmp1> zero_indeg_instr;
static int sq_max_len = 0;



static pair<int, int> find_fit(pair<int, int> pr, int start, int cycle, int read_cycle, int write_cycle) {
	//make sure start >= 0
	if (start < 0) start = 0;
	
	int best_fit = 0x3f3f3f3f, no_func;
	int i, j, k;
	bool flag;

	for (i = pr.second; i >= pr.first; i--) {
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
		schedule_queue_r[no_func][i] = x;
	for (int i = time_pos + (x->cycle) - (x->w_cycle); i < time_pos + x->cycle; i++)
		schedule_queue_w[no_func][i] = x;

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
	if (no_func != func_no["SIEU"].first && no_func != func_no["VIEU"].first) {
		int tail = strlen(x->instr_name);
		x->instr_name[tail++] = '.';
		x->instr_name[tail++] = 'M';
		x->instr_name[tail++] = no_func - func_no[x->func_unit].first + '1';
		x->instr_name[tail] = '\0';
	}
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
	int snop = 0;
	int ii;

	for (int j = 0; j < sq_max_len; j++) {
		SNOP = 1;
		first_instr = 1;
		for (int i = 0; i < num_of_func_unit; i++) {
			if (schedule_queue_r[i][j] == NULL) continue;

			SNOP = 0;
			x = schedule_queue_r[i][j];
			unplace(x, i, j);

			if (first_instr) {
				first_instr = 0;
				if (snop) {
					fprintf(fp, "\t\tSNOP\t\t%d\n", snop);
					snop = 0;
				}
			}
			else fputc('|', fp);

			if (x->cond[0] != '\0')
				fprintf(fp, "\t[%s]\t%s", x->cond, x->instr_name);
			else
				fprintf(fp, "\t\t%s", x->instr_name);
			if (!strcmp(x->func_unit, "M1")) fprintf(fp, ".%s", x->func_unit);

			if (x->input1[0] != '\0')
			{
				int flag = findchar(x->input1, ':');
				if (flag == -1) fprintf(fp, "\t%s", x->input1);
				else {
					int len = x->input1[flag + 1] - '0';
					char prefix[8];
					for (ii = 0; isalpha(x->input1[ii]); ii++)
						prefix[ii] = x->input1[ii];
					prefix[ii] = '\0';
					char reg[8];
					strncpy(reg, x->input1+ii, flag-ii);
					reg[flag - ii] = '\0';
					int reg_num = atoi(reg);
					fprintf(fp, "\t");
					for (ii = len-1; ii >= 0; ii--) {
						fprintf(fp, "%s%d", prefix, reg_num + ii);
						if (ii != 0) fprintf(fp, ":");
					}
				}

			}
			if (x->input2[0] != '\0')
			{
				int flag = findchar(x->input2, ':');
				if (flag == -1) fprintf(fp, ", %s", x->input2);
				else {
					int len = x->input2[flag + 1] - '0';
					char prefix[8];
					for (ii = 0; isalpha(x->input2[ii]); ii++)
						prefix[ii] = x->input2[ii];
					prefix[ii] = '\0';
					char reg[8];
					strncpy(reg, x->input2 + ii, flag - ii);
					reg[flag - ii] = '\0';
					int reg_num = atoi(reg);
					fprintf(fp, ", ");
					for (ii = len - 1; ii >= 0; ii--) {
						fprintf(fp, "%s%d", prefix, reg_num + ii);
						if (ii != 0) fprintf(fp, ":");
					}
				}
			}			
			if (x->input3[0] != '\0')
			{
				int flag = findchar(x->input3, ':');
				if (flag == -1) fprintf(fp, ", %s", x->input3);
				else {
					int len = x->input3[flag + 1] - '0';
					char prefix[8];
					for (ii = 0; isalpha(x->input3[ii]); ii++)
						prefix[ii] = x->input3[ii];
					prefix[ii] = '\0';
					char reg[8];
					strncpy(reg, x->input3 + ii, flag - ii);
					reg[flag - ii] = '\0';
					int reg_num = atoi(reg);
					fprintf(fp, ", ");
					for (ii = len - 1; ii >= 0; ii--) {
						fprintf(fp, "%s%d", prefix, reg_num + ii);
						if (ii != 0) fprintf(fp, ":");
					}
				}
			}
			if (x->output1[0] != '\0')
			{
				int flag = findchar(x->output1, ':');
				if (flag == -1) fprintf(fp, ", %s", x->output1);
				else {
					int len = x->output1[flag + 1] - '0';
					char prefix[8];
					for (ii = 0; isalpha(x->output1[ii]); ii++)
						prefix[ii] = x->output1[ii];
					prefix[ii] = '\0';
					char reg[8];
					strncpy(reg, x->output1 + ii, flag - ii);
					reg[flag - ii] = '\0';
					int reg_num = atoi(reg);
					fprintf(fp, ", ");
					for (ii = len - 1; ii >= 0; ii--) {
						fprintf(fp, "%s%d", prefix, reg_num + ii);
						if (ii != 0) fprintf(fp, ":");
					}
				}
			}

			fputc('\n', fp);
		}
		if (SNOP) snop++;
	}
	if (snop > 0) fprintf(fp, "\t\tSNOP\t\t%d\n", snop);
}


void Topograph::reschedule(FILE* fp) {
	Instr* x, * sbr = NULL;
	Instr* swait = NULL;
	int pos;
	pair<int, int> pr;

	load_zero_indeg_instr(this);

	while (zero_indeg_instr.size()) {
		x = zero_indeg_instr.top();
		zero_indeg_instr.pop();

		
		// if this instruction is SNOP
		if (!strcmp(x->instr_name, "SNOP"))
			continue;
		if (!strcmp(x->instr_name, "SBR")) {
			sbr = x;
			continue;
		}
		if (!strcmp(x->instr_name, "SWAIT")) {
			swait = x;
			continue;
		}
		pr = func_no[string(x->func_unit)];
		if (!strcmp(x->instr_name, "SMVAGA36")) {
			int i = 0, ii = 0;
			char cnum[10];
			int inum = 0;
			while (isalpha(x->output1[i])) i++;
			for (ii = 0; isdigit(x->output1[i]); i++, ii++)
				cnum[ii] = x->output1[i];
			cnum[ii] = '\0';
			inum = atoi(cnum);
			if (inum < 8) pr = make_pair(0, 0);
		}


		pr = find_fit(pr, x->last_fa_end_time, x->cycle, x->r_cycle, x->w_cycle);
		if (!strncmp(x->func_unit, "M1/M2", 5)) realloc_mac(x, pr.first);
		place(x, pr.first, pr.second);
	}

	if (sbr != NULL) {
		pr = func_no[string(sbr->func_unit)];
		pr = find_fit(pr, max(sbr->last_fa_end_time, sq_max_len - (sbr->cycle)), sbr->cycle, sbr->r_cycle, sbr->w_cycle);
		place(sbr, pr.first, pr.second);
	}
	if (swait != NULL) {
		pr = func_no[string(swait->func_unit)];
		pr = find_fit(pr, max(swait->last_fa_end_time, sq_max_len), swait->cycle, swait->r_cycle, swait->w_cycle);
		place(swait, pr.first, pr.second);
	}

	output_reschedule_result(fp);
}



static bool Instr_cmp(const Instr* x, const Instr* y) {
	return x->indeg < y->indeg;
}


void Topograph::output_topo_graph(FILE* fp) {
	Instr* x;
	int no, id = 0;

	//printf("reg_written info:\n");
	//for (unordered_map<reg_info, Instr*, reg_info_cmp>::iterator it = this->reg_written.begin(); it != this->reg_written.end(); it++) {
	//	x = it->second;
	//	printf("%s", x->instr_str);
	//}puts("");

	//sort(this->container.begin(), this->container.end(), Instr_cmp);
	for (int i = 0, len = this->container.size(); i < len; i++) {
		x = this->container[i];
		if (x->cycle == 0) continue;
		x->last_fa_end_time = id++;
	}
	for (int i = 0, len = this->container.size(); i < len; i++) {
		x = this->container[i];
		if (x->cycle == 0) continue;
		// fprintf(fp, "%schildren: ", x->instr_str);
		fprintf(fp, "%d:%schildren: ", x->last_fa_end_time, x->instr_str);
		for (int ii = 0, len2 = x->chl.size(); ii < len2; ii++) {
			no = x->chl[ii].first->last_fa_end_time;
			// fprintf(fp, "%d ", no);
			fprintf(fp, "(%d, %d) ", no, x->chl[ii].second);
		}// fprintf(fp, "\n");
		fprintf(fp, "\n");
	}
	// fprintf(fp, "\n");
	fprintf(fp, "\n");
}