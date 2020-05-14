#pragma once
#include <cstdio>
#include <vector>
#include <cstring>
#include <queue>
#include "analyser.h"
#include "schedule.h"
using namespace std;



inline bool isalpha(char x) {
	return ('a' <= x && x <= 'z') || ('A' <= x && x <= 'Z');
}
inline bool isdigit(char x) {
	return '0' <= x && x <= '9';
}
inline bool isaord(char x) {
	return isalpha(x) || isdigit(x);
}
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

void add_num_after_str(char* destination, char* source, int num) {
	strcpy(destination, source);
	sprintf(destination + strlen(destination), "%d", num);
}

void Topograph::push_to_cd_buff(Instr* x) {

	int i, ii, inum, pos;

	if (!strcmp(x->output1, "")) return;
	// output1 is a register
	if (x->output1[0] != '*') {
		pos = strlen(x->output1);
		if (x->output1[pos - 2] != ':') {
			reg_info res(x->output1);
			this->reg_read[res] = x;
			cd_Instr cdi(x, this->Timer + x->cycle);
			(this->cd_buff).push(cdi);
		}
		else {
			char alpha[8], cnum[8];
			for (i = 0; isalpha(x->output1[i]); i++)
				alpha[i] = x->output1[i];
			alpha[i] = '\0';
			for (ii = 0; isdigit(x->output1[i]); i++, ii++)
				cnum[ii] = x->output1[i];
			cnum[ii] = '\0';
			inum = atoi(cnum);

			reg_info res;
			for (i = x->output1[pos - 1] - '0'; i; i--) {
				add_num_after_str(res.name, alpha, inum - i + 1);
				this->reg_read[res] = x;
				cd_Instr cdi(x, this->Timer + x->cycle);
				(this->cd_buff).push(cdi);
			}
		}
	}
	// output1 is a memory_unit
	else {

		// judge AR self_change
		char AR[8], OR[16];
		int offset;
		bool self_change;
		pos = max(findchar(x->output1, '-'), findchar(x->output1, '+'));
		if (pos == -1) self_change = false;
		else if (x->output1[pos + 1] == '+' || x->output1[pos + 1] == '-') self_change = true;
		else self_change = false;

		// get AR, OR and offset
		if (pos == -1) {
			strcpy(AR, x->output1 + 1);
			reg_info ri_AR(AR);
			OR[0] = '\0';
			offset = this->reg_offset[ri_AR];
		}
		else if (self_change == true) {
			if (pos == 1) {
				i = findchar(x->output1, '[');
				strncpy(AR, x->output1 + pos + 2, i - pos - 2);
				AR[i] = '\0';
				ii = findchar(x->output1, ']');
				strncpy(OR, x->output1 + i + 1, ii - i - 1);
				OR[ii] = '\0';
			}
			else {
				strncpy(AR, x->output1 + 1, pos - 1);
				AR[pos] = '\0';
				ii = findchar(x->output1, ']');
				strncpy(OR, x->output1 + pos + 3, ii - pos - 3);
				OR[ii] = '\0';
			}
		}
		else {
			if (pos == 1) {
				i = findchar(x->output1, '[');
				strncpy(AR, x->output1 + pos + 1, i - pos - 1);
				AR[i] = '\0';
				ii = findchar(x->output1, ']');
				strncpy(OR, x->output1 + i + 1, ii - i - 1);
				OR[ii] = '\0';
			}
			else {
				strncpy(AR, x->output1 + 1, pos - 1);
				AR[pos] = '\0';
				ii = findchar(x->output1, ']');
				strncpy(OR, x->output1 + pos + 2, ii - pos - 2);
				OR[ii] = '\0';
			}
		}
		if (isdigit(OR[0])) {
			offset = stoll(OR);
			reg_info ri_AR(AR);
			inum = this->reg_offset[ri_AR];
			offset += x->output1[1] == '+' ? inum : -inum;
		}
		else offset = 0;

		// fresh reg_offset
		if (self_change == true) 
			this->reg_offset[reg_info(AR)] = offset;

		// pack this Instr
		mem_info res(AR, OR, offset);
		this->mem_read[res] = x;
		cd_Instr cdi(x, this->Timer + x->cycle, offset);
		this->cd_buff.push(cdi);
	}


	if (!strcmp(x->output2, "")) return;
	// output2 is a register
	if (x->output2[0] != '*') {
		pos = strlen(x->output2);
		if (x->output2[pos - 2] != ':') {
			reg_info res(x->output2);
			this->reg_read[res] = x;
			cd_Instr cdi(x, this->Timer + x->cycle);
			(this->cd_buff).push(cdi);
		}
		else {
			char alpha[8], cnum[8];
			for (i = 0; isalpha(x->output2[i]); i++)
				alpha[i] = x->output2[i];
			alpha[i] = '\0';
			for (ii = 0; isdigit(x->output2[i]); i++, ii++)
				cnum[ii] = x->output2[i];
			cnum[ii] = '\0';
			inum = atoi(cnum);

			reg_info res;
			for (i = x->output2[pos - 1] - '0'; i; i--) {
				add_num_after_str(res.name, alpha, inum - i + 1);
				this->reg_read[res] = x;
				cd_Instr cdi(x, this->Timer + x->cycle);
				(this->cd_buff).push(cdi);
			}
		}
	}
	// output2 is a memory_unit
	else {

		// judge AR self_change
		char AR[8], OR[16];
		int offset;
		bool self_change;
		pos = max(findchar(x->output2, '-'), findchar(x->output2, '+'));
		if (pos == -1) self_change = false;
		else if (x->output2[pos + 1] == '+' || x->output2[pos + 1] == '-') self_change = true;
		else self_change = false;

		// get AR, OR and offset
		if (pos == -1) {
			strcpy(AR, x->output2 + 1);
			reg_info ri_AR(AR);
			OR[0] = '\0';
			offset = this->reg_offset[ri_AR];
		}
		else if (self_change == true) {
			if (pos == 1) {
				i = findchar(x->output2, '[');
				strncpy(AR, x->output2 + pos + 2, i - pos - 2);
				AR[i] = '\0';
				ii = findchar(x->output2, ']');
				strncpy(OR, x->output2 + i + 1, ii - i - 1);
				OR[ii] = '\0';
			}
			else {
				strncpy(AR, x->output2 + 1, pos - 1);
				AR[pos] = '\0';
				ii = findchar(x->output2, ']');
				strncpy(OR, x->output2 + pos + 3, ii - pos - 3);
				OR[ii] = '\0';
			}
		}
		else {
			if (pos == 1) {
				i = findchar(x->output2, '[');
				strncpy(AR, x->output2 + pos + 1, i - pos - 1);
				AR[i] = '\0';
				ii = findchar(x->output2, ']');
				strncpy(OR, x->output2 + i + 1, ii - i - 1);
				OR[ii] = '\0';
			}
			else {
				strncpy(AR, x->output2 + 1, pos - 1);
				AR[pos] = '\0';
				ii = findchar(x->output2, ']');
				strncpy(OR, x->output2 + pos + 2, ii - pos - 2);
				OR[ii] = '\0';
			}
		}
		if (isdigit(OR[0])) {
			offset = stoll(OR);
			reg_info ri_AR(AR);
			inum = this->reg_offset[ri_AR];
			offset += x->output2[1] == '+' ? inum : -inum;
		}
		else offset = 0;

		// fresh reg_offset
		if (self_change == true)
			this->reg_offset[reg_info(AR)] = offset;

		// pack this Instr
		mem_info res(AR, OR, offset);
		this->mem_read[res] = x;
		cd_Instr cdi(x, this->Timer + x->cycle, offset);
		this->cd_buff.push(cdi);
	}
}

void Topograph::fresh_cd_buff() {

	this->Timer++; // Timer start from 1

	int i, ii, inum, len;
	while (!(this->cd_buff).empty() && ((this->cd_buff).top()).counter <= (this->Timer)) {
		Instr* tmp = (cd_buff.top()).instr_p;
		long long AR_offset = (cd_buff.top()).AR_offset;
		cd_buff.pop();

		if (!strcmp(tmp->output1, "")) continue;
		if (tmp->output1[0] != '*') {
			len = strlen(tmp->output1);
			if (tmp->output1[len - 2] != ':') {
				reg_info res(tmp->output1);
				this->reg_written[res] = tmp;
				this->reg_read[res] = NULL;
			}
			else {
				char alpha[8], cnum[8];
				for (i = 0; isalpha(tmp->output1[i]); i++)
					alpha[i] = tmp->output1[i];
				alpha[i] = '\0';
				for (ii = 0; isdigit(tmp->output1[i]); i++, ii++)
					cnum[ii] = tmp->output1[i];
				cnum[ii] = '\0';
				inum = atoi(cnum);

				reg_info res;
				for (i = tmp->output1[len - 1] - '0'; i; i--) {
					add_num_after_str(res.name, alpha, inum-i+1);
					this->reg_written[res] = tmp;
					this->reg_read[res] = NULL;
				}
			}
		}
		else {
			char AR[8], OR[16];
			if (isalpha(tmp->output1[1])) {
				int self_change;
				if ((self_change = findchar(tmp->output1, '+')) != -1 || (self_change = findchar(tmp->output1, '-')) != -1) {
					strncpy(AR, tmp->output1 + 1, self_change - 1);
					AR[self_change-1] = '\0';

					if (isdigit(tmp->output1[self_change + 3])) {
						mem_info res(AR, NULL, AR_offset);
						this->mem_written[res] = tmp;
						this->mem_read[res] = NULL;
					}
					else {
						for (ii = 0, i = self_change + 3; isaord(tmp->output1[i]); i++, ii++)
							OR[ii] = tmp->output1[i];
						OR[ii] = '\0';
						mem_info res(AR, OR, 0);
						this->mem_written[res] = tmp;
						this->mem_read[res] = NULL;
					}
				}
				else {
					strcpy(AR, tmp->output1 + 1);
					mem_info res(AR, NULL, AR_offset);
					this->mem_written[res] = tmp;
					this->mem_read[res] = NULL;
				}
			}
			else{
				i = isalpha(tmp->output1[2])? 2 : 3;
				for (ii = 0; isaord(tmp->output1[i]); i++, ii++)
					AR[ii] = tmp->output1[i];
				AR[ii] = '\0';
				if (isdigit(tmp->output1[++i])) {
					mem_info res(AR, NULL, AR_offset);
					this->mem_written[res] = tmp;
					this->mem_read[res] = NULL;
				}
				else {
					for (ii = 0; isaord(tmp->output1[i]); i++, ii++)
						OR[ii] = tmp->output1[i];
					OR[ii] = '\0';
					mem_info res(AR, OR, 0);
					this->mem_written[res] = tmp;
					this->mem_read[res] = NULL;
				}
			}
		}


		if (!strcmp(tmp->output2, "")) continue;
		if (tmp->output2[0] != '*') {
			len = strlen(tmp->output2);
			if (tmp->output2[len - 2] != ':') {
				reg_info res(tmp->output2);
				this->reg_written[res] = tmp;
				this->reg_read[res] = NULL;
			}
			else {
				char alpha[8], cnum[8];
				for (i = 0; isalpha(tmp->output2[i]); i++)
					alpha[i] = tmp->output2[i];
				alpha[i] = '\0';
				for (ii = 0; isdigit(tmp->output2[i]); i++, ii++)
					cnum[ii] = tmp->output2[i];
				cnum[ii] = '\0';
				inum = atoi(cnum);

				reg_info res;
				for (i = tmp->output2[len - 1] - '0'; i; i--) {
					add_num_after_str(res.name, alpha, inum - i + 1);
					this->reg_written[res] = tmp;
					this->reg_read[res] = NULL;
				}
			}
		}
		else {
			char AR[8], OR[16];
			if (isalpha(tmp->output2[1])) {
				int self_change;
				if ((self_change = findchar(tmp->output2, '+')) != -1 || (self_change = findchar(tmp->output2, '-')) != -1) {
					strncpy(AR, tmp->output2 + 1, self_change - 1);
					AR[self_change - 1] = '\0';

					if (isdigit(tmp->output2[self_change + 3])) {
						mem_info res(AR, NULL, AR_offset);
						this->mem_written[res] = tmp;
						this->mem_read[res] = NULL;
					}
					else {
						for (ii = 0, i = self_change + 3; isaord(tmp->output2[i]); i++, ii++)
							OR[ii] = tmp->output2[i];
						OR[ii] = '\0';
						mem_info res(AR, OR, 0);
						this->mem_written[res] = tmp;
						this->mem_read[res] = NULL;
					}
				}
				else {
					strcpy(AR, tmp->output2 + 1);
					mem_info res(AR, NULL, AR_offset);
					this->mem_written[res] = tmp;
					this->mem_read[res] = NULL;
				}
			}
			else {
				i = isalpha(tmp->output2[2]) ? 2 : 3;
				for (ii = 0; isaord(tmp->output2[i]); i++, ii++)
					AR[ii] = tmp->output2[i];
				AR[ii] = '\0';
				if (isdigit(tmp->output2[++i])) {
					mem_info res(AR, NULL, AR_offset);
					this->mem_written[res] = tmp;
					this->mem_read[res] = NULL;
				}
				else {
					for (ii = 0; isaord(tmp->output2[i]); i++, ii++)
						OR[ii] = tmp->output2[i];
					OR[ii] = '\0';
					mem_info res(AR, OR, 0);
					this->mem_written[res] = tmp;
					this->mem_read[res] = NULL;
				}
			}
		}

	}
}


bool mem_rely(const mem_info& fa,const mem_info& chl) {
	if (isalpha(chl.OR[0]) || isalpha(fa.OR[0])) return true;
	else if (fa.AR_offset == chl.AR_offset) return true;
	else return false;
}

void Topograph::build_dependency(Instr* x) {
	int i, ii, inum, len;
	long long offset;
	char alpha[8], cnum[8], AR[8], OR[16];

	if (strcmp(x->input1, "") && !isdigit(x->input1[0])) {
		// input1 is a register
		if (x->input1[0] != '*') {
			len = strlen(x->input1);
			// single register
			if (x->input1[len - 2] != ':') {
				reg_info ri(x->input1);
				Instr* ptr = this->reg_written[ri];
				if (ptr != NULL) {
					ptr->chl.push_back(make_pair(x, ptr->cycle));
					x->indeg++;
				}
			}
			// register pairs
			else {
				for (i = 0; isalpha(x->input1[i]); i++)
					alpha[i] = x->input1[i];
				alpha[i] = '\0';
				for (ii = 0; isdigit(x->input1[i]); i++, ii++)
					cnum[ii] = x->input1[i];
				cnum[ii] = '\0';
				inum = atoi(cnum);
				for (i = x->input1[len - 1] - '0'; i; i--) {
					add_num_after_str(OR, alpha, inum - i + 1);
					reg_info ri(OR);
					Instr* ptr = this->reg_written[ri];
					if (ptr != NULL) {
						ptr->chl.push_back(make_pair(x, ptr->cycle));
						x->indeg++;
					}
				}
			}
		}
		// input1 is a memory_unit
		else {
			bool self_change;
			len = max(findchar(x->input1, '-'), findchar(x->input1, '+'));
			if (len == -1) self_change = false;
			else if (x->input1[len + 1] == '+' || x->input1[len + 1] == '-') self_change = true;
			else self_change = false;

			// get AR, OR and offset
			if (len == -1) {
				strcpy(AR, x->input1 + 1);
				reg_info ri_AR(AR);
				OR[0] = '\0';
				offset = this->reg_offset[ri_AR];
			}
			else if (self_change == true) {
				if (len == 1) {
					i = findchar(x->input1, '[');
					strncpy(AR, x->input1 + len + 2, i - len - 2);
					AR[i] = '\0';
					ii = findchar(x->input1, ']');
					strncpy(OR, x->input1 + i + 1, ii - i - 1);
					OR[ii] = '\0';
				}
				else {
					strncpy(AR, x->input1 + 1, len - 1);
					AR[len] = '\0';
					ii = findchar(x->input1, ']');
					strncpy(OR, x->input1 + len + 3, ii - len - 3);
					OR[ii] = '\0';
				}
			}
			else {
				if (len == 1) {
					i = findchar(x->input1, '[');
					strncpy(AR, x->input1 + len + 1, i - len - 1);
					AR[i] = '\0';
					ii = findchar(x->input1, ']');
					strncpy(OR, x->input1 + i + 1, ii - i - 1);
					OR[ii] = '\0';
				}
				else {
					strncpy(AR, x->input1 + 1, len - 1);
					AR[len] = '\0';
					ii = findchar(x->input1, ']');
					strncpy(OR, x->input1 + len + 2, ii - len - 2);
					OR[ii] = '\0';
				}
			}
			if (isdigit(OR[0])) {
				offset = stoll(OR);
				reg_info ri_AR(AR);
				inum = this->reg_offset[ri_AR];
				offset += x->input1[1] == '+' ? inum : -inum;
			}
			else offset = 0;

			mem_info this_mem(AR, OR, offset);
			for (map<mem_info, Instr*, mem_info_cmp>::iterator it = this->mem_written.begin(); it != this->mem_written.end();) {
				if (it->second != NULL && mem_rely(it->first, this_mem)) {
					((it->second)->chl).push_back(make_pair(x, it->second->cycle));
					(this->mem_written).erase(it++);
				}
				else {
					it++;
				}
			}
		}
	}


	if (strcmp(x->input2, "") && !isdigit(x->input2[0])) {
		// input2 is a register
		if (x->input2[0] != '*') {
			len = strlen(x->input2);
			// single register
			if (x->input2[len - 2] != ':') {
				reg_info ri(x->input2);
				Instr* ptr = this->reg_written[ri];
				if (ptr != NULL) {
					ptr->chl.push_back(make_pair(x, ptr->cycle));
					x->indeg++;
				}
			}
			// register pairs
			else {
				for (i = 0; isalpha(x->input2[i]); i++)
					alpha[i] = x->input2[i];
				alpha[i] = '\0';
				for (ii = 0; isdigit(x->input2[i]); i++, ii++)
					cnum[ii] = x->input2[i];
				cnum[ii] = '\0';
				inum = atoi(cnum);
				for (i = x->input2[len - 1] - '0'; i; i--) {
					add_num_after_str(OR, alpha, inum - i + 1);
					reg_info ri(OR);
					Instr* ptr = this->reg_written[ri];
					if (ptr != NULL) {
						ptr->chl.push_back(make_pair(x, ptr->cycle));
						x->indeg++;
					}
				}
			}
		}
		// input2 is a memory_unit
		else {
			bool self_change;
			len = max(findchar(x->input2, '-'), findchar(x->input2, '+'));
			if (len == -1) self_change = false;
			else if (x->input2[len + 1] == '+' || x->input2[len + 1] == '-') self_change = true;
			else self_change = false;

			// get AR, OR and offset
			if (len == -1) {
				strcpy(AR, x->input2 + 1);
				reg_info ri_AR(AR);
				OR[0] = '\0';
				offset = this->reg_offset[ri_AR];
			}
			else if (self_change == true) {
				if (len == 1) {
					i = findchar(x->input2, '[');
					strncpy(AR, x->input2 + len + 2, i - len - 2);
					AR[i] = '\0';
					ii = findchar(x->input2, ']');
					strncpy(OR, x->input2 + i + 1, ii - i - 1);
					OR[ii] = '\0';
				}
				else {
					strncpy(AR, x->input2 + 1, len - 1);
					AR[len] = '\0';
					ii = findchar(x->input2, ']');
					strncpy(OR, x->input2 + len + 3, ii - len - 3);
					OR[ii] = '\0';
				}
			}
			else {
				if (len == 1) {
					i = findchar(x->input2, '[');
					strncpy(AR, x->input2 + len + 1, i - len - 1);
					AR[i] = '\0';
					ii = findchar(x->input2, ']');
					strncpy(OR, x->input2 + i + 1, ii - i - 1);
					OR[ii] = '\0';
				}
				else {
					strncpy(AR, x->input2 + 1, len - 1);
					AR[len] = '\0';
					ii = findchar(x->input2, ']');
					strncpy(OR, x->input2 + len + 2, ii - len - 2);
					OR[ii] = '\0';
				}
			}
			if (isdigit(OR[0])) {
				offset = stoll(OR);
				reg_info ri_AR(AR);
				inum = this->reg_offset[ri_AR];
				offset += x->input2[1] == '+' ? inum : -inum;
			}
			else offset = 0;

			mem_info this_mem(AR, OR, offset);
			for (map<mem_info, Instr*, mem_info_cmp>::iterator it = this->mem_written.begin(); it != this->mem_written.end();) {
				if (it->second != NULL && mem_rely(it->first, this_mem)) {
					((it->second)->chl).push_back(make_pair(x, it->second->cycle));
					(this->mem_written).erase(it++);
				}
				else {
					it++;
				}
			}
		}
	}


	if (strcmp(x->input3, "") && !isdigit(x->input3[0])) {
		// input3 is a register
		if (x->input3[0] != '*') {
			len = strlen(x->input3);
			// single register
			if (x->input3[len - 2] != ':') {
				reg_info ri(x->input3);
				Instr* ptr = this->reg_written[ri];
				if (ptr != NULL) {
					ptr->chl.push_back(make_pair(x, ptr->cycle));
					x->indeg++;
				}
			}
			// register pairs
			else {
				for (i = 0; isalpha(x->input3[i]); i++)
					alpha[i] = x->input3[i];
				alpha[i] = '\0';
				for (ii = 0; isdigit(x->input3[i]); i++, ii++)
					cnum[ii] = x->input3[i];
				cnum[ii] = '\0';
				inum = atoi(cnum);
				for (i = x->input3[len - 1] - '0'; i; i--) {
					add_num_after_str(OR, alpha, inum - i + 1);
					reg_info ri(OR);
					Instr* ptr = this->reg_written[ri];
					if (ptr != NULL) {
						ptr->chl.push_back(make_pair(x, ptr->cycle));
						x->indeg++;
					}
				}
			}
		}
		// input3 is a memory_unit
		else {
			bool self_change;
			len = max(findchar(x->input3, '-'), findchar(x->input3, '+'));
			if (len == -1) self_change = false;
			else if (x->input3[len + 1] == '+' || x->input3[len + 1] == '-') self_change = true;
			else self_change = false;

			// get AR, OR and offset
			if (len == -1) {
				strcpy(AR, x->input3 + 1);
				reg_info ri_AR(AR);
				OR[0] = '\0';
				offset = this->reg_offset[ri_AR];
			}
			else if (self_change == true) {
				if (len == 1) {
					i = findchar(x->input3, '[');
					strncpy(AR, x->input3 + len + 2, i - len - 2);
					AR[i] = '\0';
					ii = findchar(x->input3, ']');
					strncpy(OR, x->input3 + i + 1, ii - i - 1);
					OR[ii] = '\0';
				}
				else {
					strncpy(AR, x->input3 + 1, len - 1);
					AR[len] = '\0';
					ii = findchar(x->input3, ']');
					strncpy(OR, x->input3 + len + 3, ii - len - 3);
					OR[ii] = '\0';
				}
			}
			else {
				if (len == 1) {
					i = findchar(x->input3, '[');
					strncpy(AR, x->input3 + len + 1, i - len - 1);
					AR[i] = '\0';
					ii = findchar(x->input3, ']');
					strncpy(OR, x->input3 + i + 1, ii - i - 1);
					OR[ii] = '\0';
				}
				else {
					strncpy(AR, x->input3 + 1, len - 1);
					AR[len] = '\0';
					ii = findchar(x->input3, ']');
					strncpy(OR, x->input3 + len + 2, ii - len - 2);
					OR[ii] = '\0';
				}
			}
			if (isdigit(OR[0])) {
				offset = stoll(OR);
				reg_info ri_AR(AR);
				inum = this->reg_offset[ri_AR];
				offset += x->input3[1] == '+' ? inum : -inum;
			}
			else offset = 0;

			mem_info this_mem(AR, OR, offset);
			for (map<mem_info, Instr*, mem_info_cmp>::iterator it = this->mem_written.begin(); it != this->mem_written.end();) {
				if (it->second != NULL && mem_rely(it->first, this_mem)) {
					((it->second)->chl).push_back(make_pair(x, it->second->cycle));
					(this->mem_written).erase(it++);
				}
				else {
					it++;
				}
			}
		}
	}


	if (strcmp(x->output1, "")) {
		// output1 is a register
		if (x->output1[0] != '*') {
			len = strlen(x->output1);
			// single register
			if (x->output1[len - 2] != ':') {
				reg_info ri(x->output1);
				Instr* ptr = this->reg_read[ri];
				if (ptr != NULL) {
					x->chl.push_back(make_pair(ptr, -(ptr->cycle)));
					ptr->indeg++;
				}
			}
			// register pairs
			else {
				for (i = 0; isalpha(x->output1[i]); i++)
					alpha[i] = x->output1[i];
				alpha[i] = '\0';
				for (ii = 0; isdigit(x->output1[i]); i++, ii++)
					cnum[ii] = x->output1[i];
				cnum[ii] = '\0';
				inum = atoi(cnum);
				for (i = x->output1[len - 1] - '0'; i; i--) {
					add_num_after_str(OR, alpha, inum - i + 1);
					reg_info ri(OR);
					Instr* ptr = this->reg_read[ri];
					if (ptr != NULL) {
						x->chl.push_back(make_pair(ptr, -(ptr->cycle)));
						ptr->indeg++;
					}
				}
			}
		}
		// output1 is a memory_unit
		else {
			bool self_change;
			len = max(findchar(x->output1, '-'), findchar(x->output1, '+'));
			if (len == -1) self_change = false;
			else if (x->output1[len + 1] == '+' || x->output1[len + 1] == '-') self_change = true;
			else self_change = false;

			// get AR, OR and offset
			if (len == -1) {
				strcpy(AR, x->output1 + 1);
				reg_info ri_AR(AR);
				OR[0] = '\0';
				offset = this->reg_offset[ri_AR];
			}
			else if (self_change == true) {
				if (len == 1) {
					i = findchar(x->output1, '[');
					strncpy(AR, x->output1 + len + 2, i - len - 2);
					AR[i] = '\0';
					ii = findchar(x->output1, ']');
					strncpy(OR, x->output1 + i + 1, ii - i - 1);
					OR[ii] = '\0';
				}
				else {
					strncpy(AR, x->output1 + 1, len - 1);
					AR[len] = '\0';
					ii = findchar(x->output1, ']');
					strncpy(OR, x->output1 + len + 3, ii - len - 3);
					OR[ii] = '\0';
				}
			}
			else {
				if (len == 1) {
					i = findchar(x->output1, '[');
					strncpy(AR, x->output1 + len + 1, i - len - 1);
					AR[i] = '\0';
					ii = findchar(x->output1, ']');
					strncpy(OR, x->output1 + i + 1, ii - i - 1);
					OR[ii] = '\0';
				}
				else {
					strncpy(AR, x->output1 + 1, len - 1);
					AR[len] = '\0';
					ii = findchar(x->output1, ']');
					strncpy(OR, x->output1 + len + 2, ii - len - 2);
					OR[ii] = '\0';
				}
			}
			if (isdigit(OR[0])) {
				offset = stoll(OR);
				reg_info ri_AR(AR);
				inum = this->reg_offset[ri_AR];
				offset += x->output1[1] == '+' ? inum : -inum;
			}
			else offset = 0;

			mem_info this_mem(AR, OR, offset);
			for (map<mem_info, Instr*, mem_info_cmp>::iterator it = this->mem_read.begin(); it != this->mem_read.end();) {
				if (it->second != NULL && mem_rely(it->first, this_mem)) {
					(x->chl).push_back(make_pair(it->second, -((it->second)->cycle)));
					this->mem_read.erase(it++);
				}
				else {
					it++;
				}
			}
		}
	}


	if (strcmp(x->output2, "")) {
		// output2 is a register
		if (x->output2[0] != '*') {
			len = strlen(x->output2);
			// single register
			if (x->output2[len - 2] != ':') {
				reg_info ri(x->output2);
				Instr* ptr = this->reg_read[ri];
				if (ptr != NULL) {
					x->chl.push_back(make_pair(ptr, -(ptr->cycle)));
					ptr->indeg++;
				}
			}
			// register pairs
			else {
				for (i = 0; isalpha(x->output2[i]); i++)
					alpha[i] = x->output2[i];
				alpha[i] = '\0';
				for (ii = 0; isdigit(x->output2[i]); i++, ii++)
					cnum[ii] = x->output2[i];
				cnum[ii] = '\0';
				inum = atoi(cnum);
				for (i = x->output2[len - 1] - '0'; i; i--) {
					add_num_after_str(OR, alpha, inum - i + 1);
					reg_info ri(OR);
					Instr* ptr = this->reg_read[ri];
					if (ptr != NULL) {
						x->chl.push_back(make_pair(ptr, -(ptr->cycle)));
						ptr->indeg++;
					}
				}
			}
		}
		// output2 is a memory_unit
		else {
			bool self_change;
			len = max(findchar(x->output2, '-'), findchar(x->output2, '+'));
			if (len == -1) self_change = false;
			else if (x->output2[len + 1] == '+' || x->output2[len + 1] == '-') self_change = true;
			else self_change = false;

			// get AR, OR and offset
			if (len == -1) {
				strcpy(AR, x->output2 + 1);
				reg_info ri_AR(AR);
				OR[0] = '\0';
				offset = this->reg_offset[ri_AR];
			}
			else if (self_change == true) {
				if (len == 1) {
					i = findchar(x->output2, '[');
					strncpy(AR, x->output2 + len + 2, i - len - 2);
					AR[i] = '\0';
					ii = findchar(x->output2, ']');
					strncpy(OR, x->output2 + i + 1, ii - i - 1);
					OR[ii] = '\0';
				}
				else {
					strncpy(AR, x->output2 + 1, len - 1);
					AR[len] = '\0';
					ii = findchar(x->output2, ']');
					strncpy(OR, x->output2 + len + 3, ii - len - 3);
					OR[ii] = '\0';
				}
			}
			else {
				if (len == 1) {
					i = findchar(x->output2, '[');
					strncpy(AR, x->output2 + len + 1, i - len - 1);
					AR[i] = '\0';
					ii = findchar(x->output2, ']');
					strncpy(OR, x->output2 + i + 1, ii - i - 1);
					OR[ii] = '\0';
				}
				else {
					strncpy(AR, x->output2 + 1, len - 1);
					AR[len] = '\0';
					ii = findchar(x->output2, ']');
					strncpy(OR, x->output2 + len + 2, ii - len - 2);
					OR[ii] = '\0';
				}
			}
			if (isdigit(OR[0])) {
				offset = stoll(OR);
				reg_info ri_AR(AR);
				inum = this->reg_offset[ri_AR];
				offset += x->output2[1] == '+' ? inum : -inum;
			}
			else offset = 0;

			mem_info this_mem(AR, OR, offset);
			for (map<mem_info, Instr*, mem_info_cmp>::iterator it = this->mem_read.begin(); it != this->mem_read.end();) {
				if (it->second != NULL && mem_rely(it->first, this_mem)) {
					(x->chl).push_back(make_pair(it->second, -((it->second)->cycle)));
					this->mem_read.erase(it++);
				}
				else {
					it++;
				}
			}
		}
	}

}


void process_file(char* destination, char* source) {
	FILE *fpin, *fpout;
	fpin = fopen(source, "r");
	fpout = fopen(destination, "w");

	int st;// Abbreviation for sentence_type
	char sentence[256], buf[256];
	Instr* instr_x;

	new_topo:
	{
		Topograph topo;
		while (peek(fpin) != EOF) {
			fgets(sentence, 256, fpin);
			if (!strcmp(sentence, "")) continue;

			get_first_word(buf, sentence);
			st = sentence_type(buf, sentence);
			if (st) {
				// fprintf(fpout, "%s", sentence);
				if (st == 1) {
					topo.reschedule(fpout);
					topo.clear();
					fprintf(fpout, "%s", sentence);
					goto new_topo;
				}
				else fprintf(fpout, "%s", sentence);
				continue;
			}

			instr_x = topo.generate_Instr(sentence);
			(topo.container).push_back(instr_x);
			topo.instr_num++;
			//printf("%s", instr_x->instr_str);
			//printf("%sfunc_unit:%s   \tcycle:%d \tinput1:%s \tinput2:%s \tinput3:%s \toutput1:%s \toutput2:%s \tr_cycle:%d \tw_cycle:%d\n\n",\
				instr_x->instr_str, instr_x->func_unit, instr_x->cycle, instr_x->input1, instr_x->input2, \
				instr_x->input3, instr_x->output1, instr_x->output2, instr_x->r_cycle, instr_x->w_cycle);
			topo.fresh_cd_buff();
			topo.build_dependency(instr_x);
			topo.push_to_cd_buff(instr_x);

			//Instr* x;
			//printf("reg_written info:\n");
			//for (unordered_map<reg_info, Instr*, reg_info_cmp>::iterator it = (topo.reg_written).begin(); it != (topo.reg_written).end(); it++) {
			//	x = it->second;
			//	if (x == NULL) continue;
			//	printf("%s", x->instr_str);
			//}puts("");
		
		}
		topo.reschedule(fpout);
	}
	


	fclose(fpin);
	fclose(fpout);
}