#pragma once
#include <cstdio>
#include <vector>
#include <cstring>
#include <queue>
#include "Topo_relative.h"
using namespace std;


bool reg_equal(char* str_conj, char* str) {
	if (findchar(str_conj, ':') == -1 || str_conj[0] == '\0') return !strcmp(str_conj, str);
	char alpha[8], cnum[8];
	int inum;
	int i, ii;
	for (i = 0; isalpha(str_conj[i]); i++)
		alpha[i] = str_conj[i];
	alpha[i] = '\0';
	for (ii = 0; isdigit(str_conj[i]); i++, ii++)
		cnum[ii] = str_conj[i];
	cnum[ii] = '\0';
	inum = atoi(cnum);

	int len = strlen(str_conj);
	char reg_name[10];
	int flag = 0;
	for (i = str_conj[len - 1] - '0'; i; i--) {
		add_num_after_str(reg_name, alpha, inum + i - 1);
		if (!strcmp(reg_name, str)) flag = 1;
	}
	return flag;
}

void Topograph::push_to_cd_buff(Instr* x) {

	int i, ii, inum, pos;

	//cond field
	if (strcmp(x->cond, "") && !isdigit(x->cond[0])) {
		reg_info res(x->cond);
		//this->reg_read[res] = x;
		Instr* ptr = this->reg_written[res];
		if (ptr == NULL) {
			Instr vptr;
			vptr.init();
			vptr.instr_str = (char*)malloc(1);
			vptr.instr_str[0] = '\0';
			vptr.chl.push_back(make_pair(x, vptr.cycle));
			x->indeg++;
			this->All_Instr[this->instr_num] = vptr;
			(this->container).push_back(&this->All_Instr[this->instr_num]);
			this->reg_written[res] = &this->All_Instr[this->instr_num];
			this->instr_num++;
		}
		else {
			ptr->chl.push_back(make_pair(x, ptr->cycle));
			x->indeg++;
		}
	}

	//input1
	if (strcmp(x->input1, "") && !isdigit(x->input1[0])) {
		// input1 is a register
		if (x->input1[0] != '*') {
			build_read_reg_dependency(x, x->input1);
		}
		// input1 is a memory_unit
		else {
			build_read_mem_dependency(x, x->input1, 1);
		}
	}

	//input2
	if (strcmp(x->input2, "") && !isdigit(x->input2[0])) {
		// input2 is a register
		if (x->input2[0] != '*') {
			build_read_reg_dependency(x, x->input2);
		}
		// input2 is a memory_unit
		else {
			build_read_mem_dependency(x, x->input2, 1);
		}
	}


	//input3
	if (strcmp(x->input3, "") && !isdigit(x->input3[0])) {
		// input3 is a register
		if (x->input3[0] != '*') {
			build_read_reg_dependency(x, x->input3);
		}
		// input3 is a memory_unit
		else {
			build_read_mem_dependency(x, x->input3, 1);
		}
	}

	//output1
	if (x->output1[0] == '*') {
		build_read_mem_dependency(x, x->output1, 0);
	}

	if (!strcmp(x->output1, "")) return;
	cd_Instr cdi(x, this->Timer + x->cycle);
	this->cd_buff.push(cdi);
}


void Topograph::fresh_cd_buff() {
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

				Instr* ptr = this->reg_written[res];
				if (ptr != NULL) {
					ptr->chl.push_back(make_pair(tmp, ptr->cycle - tmp->cycle + tmp->w_cycle));
					tmp->indeg++;

					for (i = 0; i < (ptr->chl).size(); i++) {
						Instr* child = (ptr->chl[i]).first;
						if (reg_equal(child->cond, tmp->output1) || reg_equal(child->input1, tmp->output1) || reg_equal(child->input2, tmp->output1) || reg_equal(child->input3, tmp->output1)) {
							if (tmp != child) {
								child->chl.push_back(make_pair(tmp, child->r_cycle - tmp->cycle + tmp->w_cycle - 1));
								tmp->indeg++;
							}
						}
					}
				}
				this->reg_written[res] = tmp;
				if (tmp->output1[0] == 'A')
					this->up_AR_change[res] = tmp;
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
					add_num_after_str(res.name, alpha, inum + i - 1);
					Instr* ptr = this->reg_written[res];
					if (ptr != NULL) {
						ptr->chl.push_back(make_pair(tmp, ptr->cycle - tmp->cycle + tmp->w_cycle));
						tmp->indeg++;

						for (i = 0; i < (ptr->chl).size(); i++) {
							Instr* child = (ptr->chl[i]).first;
							if (reg_equal(child->cond, tmp->output1) || reg_equal(child->input1, tmp->output1) || reg_equal(child->input2, tmp->output1) || reg_equal(child->input3, tmp->output1)) {
								if (tmp != child) {
									child->chl.push_back(make_pair(tmp, child->r_cycle - tmp->cycle + tmp->w_cycle - 1));
									tmp->indeg++;
								}
							}
						}
					}
					this->reg_written[res] = tmp;
					if (alpha[0] == 'A')
						this->up_AR_change[res] = tmp;
				}
			}
		}
		else {
			char AR[8], OR[16];
			long long offset;
			bool self_change;
			len = max(findchar(tmp->output1, '-'), findchar(tmp->output1, '+'));
			if (len == -1) self_change = false;
			else if (tmp->output1[len + 1] == '+' || tmp->output1[len + 1] == '-') self_change = true;
			else self_change = false;

			// get AR, OR and offset
			if (len == -1) {
				strcpy(AR, tmp->output1 + 1);
				reg_info ri_AR(AR);
				OR[0] = '\0';
				offset = this->reg_offset[ri_AR];
			}
			else if (self_change == true) {
				if (len == 1) {
					i = findchar(tmp->output1, '[');
					strncpy(AR, tmp->output1 + len + 2, i - len - 2);
					AR[i - len - 2] = '\0';
					ii = findchar(tmp->output1, ']');
					strncpy(OR, tmp->output1 + i + 1, ii - i - 1);
					OR[ii - i - 1] = '\0';
				}
				else {
					strncpy(AR, tmp->output1 + 1, len - 1);
					AR[len - 1] = '\0';
					ii = findchar(tmp->output1, ']');
					strncpy(OR, tmp->output1 + len + 3, ii - len - 3);
					OR[ii - len - 3] = '\0';
				}
			}
			else {
				if (len == 1) {
					i = findchar(tmp->output1, '[');
					strncpy(AR, tmp->output1 + len + 1, i - len - 1);
					AR[i - len - 1] = '\0';
					ii = findchar(tmp->output1, ']');
					strncpy(OR, tmp->output1 + i + 1, ii - i - 1);
					OR[ii - i - 1] = '\0';
				}
				else {
					strncpy(AR, tmp->output1 + 1, len - 1);
					AR[len - 1] = '\0';
					ii = findchar(tmp->output1, ']');
					strncpy(OR, tmp->output1 + len + 2, ii - len - 2);
					OR[ii - len - 2] = '\0';
				}
			}
			if (isdigit(OR[0])) {
				offset = stoll(OR);
				reg_info ri_AR(AR);
				inum = this->reg_offset[ri_AR];
				offset = inum + tmp->output1[1] == '+' ? offset : -offset;
			}
			else offset = 0;

			mem_info this_mem(AR, OR, offset);
			for (map<mem_info, Instr*, mem_info_cmp>::iterator it = this->mem_read.begin(); it != this->mem_read.end();) {
				if (it->second != NULL && mem_rely(it->first, this_mem)) {
					((it->second)->chl).push_back(make_pair(tmp, it->second->r_cycle - tmp->cycle + tmp->w_cycle - 1));
					(this->mem_read).erase(it++);
					tmp->indeg++;
				}
				else {
					it++;
				}
			}
			for (map<mem_info, Instr*, mem_info_cmp>::iterator it = this->mem_written.begin(); it != this->mem_written.end();) {
				if (it->second != NULL && mem_rely(it->first, this_mem)) {
					((it->second)->chl).push_back(make_pair(tmp, it->second->cycle - tmp->cycle + tmp->w_cycle));
					(this->mem_written).erase(it++);
					tmp->indeg++;
				}
				else {
					it++;
				}
			}
			this->mem_written[this_mem] = tmp;
		}
	}
}

void Topograph::build_read_reg_dependency(Instr* x, char* input) {
	int pos = strlen(input);
	if (input[pos - 2] != ':') {
		reg_info res(input);
		//this->reg_read[res] = x;
		Instr* ptr = this->reg_written[res];
		if (ptr == NULL) {
			Instr vptr;
			vptr.init();
			vptr.instr_str = (char*)malloc(1);
			vptr.instr_str[0] = '\0';
			vptr.chl.push_back(make_pair(x, vptr.cycle));
			x->indeg++;
			this->All_Instr[this->instr_num] = vptr;
			(this->container).push_back(&this->All_Instr[this->instr_num]);
			this->reg_written[res] = &this->All_Instr[this->instr_num];
			this->instr_num++;
		}
		else {
			ptr->chl.push_back(make_pair(x, ptr->cycle));
			x->indeg++;
		}
	}
	else {// a pair of registers
		int i, ii;
		int inum;
		char alpha[8], cnum[8];
		for (i = 0; isalpha(input[i]); i++)
			alpha[i] = input[i];
		alpha[i] = '\0';
		for (ii = 0; isdigit(input[i]); i++, ii++)
			cnum[ii] = input[i];
		cnum[ii] = '\0';
		inum = atoi(cnum);

		reg_info res;
		for (i = input[pos - 1] - '0'; i; i--) {
			add_num_after_str(res.name, alpha, inum + i - 1);
			//this->reg_read[res] = x;
			Instr* ptr = this->reg_written[res];
			if (ptr == NULL) {
				Instr vptr;
				vptr.init();
				vptr.instr_str = (char*)malloc(1);
				vptr.instr_str[0] = '\0';
				vptr.chl.push_back(make_pair(x, vptr.cycle));
				x->indeg++;
				this->All_Instr[this->instr_num] = vptr;
				(this->container).push_back(&this->All_Instr[this->instr_num]);
				this->reg_written[res] = &this->All_Instr[this->instr_num];
				this->instr_num++;
			}
			else {
				ptr->chl.push_back(make_pair(x, ptr->cycle));
				x->indeg++;
			}
		}
	}
}


void Topograph::build_read_mem_dependency(Instr* x, char* input, int load_store) {

	char AR[8], OR[16];
	int i, ii, pos, offset, inum;
	bool self_change;

	// judge AR self_change
	pos = max(findchar(input, '-'), findchar(input, '+'));
	if (pos == -1) self_change = false;
	else if (input[pos + 1] == '+' || input[pos + 1] == '-') self_change = true;
	else self_change = false;

	// get AR, OR and offset
	if (pos == -1) {
		strcpy(AR, input + 1);
		reg_info ri_AR(AR);
		OR[0] = '\0';
		offset = this->reg_offset[ri_AR];
	}
	else if (self_change == true) {
		if (pos == 1) {
			i = findchar(input, '[');
			strncpy(AR, input + pos + 2, i - pos - 2);
			AR[i - pos - 2] = '\0';
			ii = findchar(input, ']');
			strncpy(OR, input + i + 1, ii - i - 1);
			OR[ii - i - 1] = '\0';
		}
		else {
			strncpy(AR, input + 1, pos - 1);
			AR[pos - 1] = '\0';
			ii = findchar(input, ']');
			strncpy(OR, input + pos + 3, ii - pos - 3);
			OR[ii - pos - 3] = '\0';
		}
	}
	else {
		if (pos == 1) {
			i = findchar(input, '[');
			strncpy(AR, input + pos + 1, i - pos - 1);
			AR[i - pos - 1] = '\0';
			ii = findchar(input, ']');
			strncpy(OR, input + i + 1, ii - i - 1);
			OR[ii - i - 1] = '\0';
		}
		else {
			strncpy(AR, input + 1, pos - 1);
			AR[pos - 1] = '\0';
			ii = findchar(input, ']');
			strncpy(OR, input + pos + 2, ii - pos - 2);
			OR[ii - pos - 2] = '\0';
		}
	}
	if (isdigit(OR[0])) {
		offset = stoll(OR);
		reg_info ri_AR(AR);
		inum = this->reg_offset[ri_AR];
		offset += input[1] == '+' ? inum : -inum;
	}
	else offset = 0;

	// fresh reg_offset
	if (self_change == true)
		this->reg_offset[reg_info(AR)] = offset;

	// all mem_read or mem_write Instr should be placed after last AR change Instr
	reg_info resA(AR);
	Instr* last_AR_change = this->up_AR_change[AR];
	if (last_AR_change != NULL) {
		if (load_store) {
			last_AR_change->chl.push_back(make_pair(x, last_AR_change->cycle));
			x->indeg++;
		}
		else {
			last_AR_change->chl.push_back(make_pair(x, last_AR_change->cycle - x->cycle + x->w_cycle));
			x->indeg++;
		}
	}


	if (!isdigit(OR[0])) {
		reg_info resO(OR);
		//this->reg_read[res] = x;
		Instr* ptr = this->reg_written[resO];
		if (ptr == NULL) {
			Instr vptr;
			vptr.init();
			vptr.instr_str = (char*)malloc(1);
			vptr.instr_str[0] = '\0';
			vptr.chl.push_back(make_pair(x, vptr.cycle));
			x->indeg++;
			this->All_Instr[this->instr_num] = vptr;
			(this->container).push_back(&this->All_Instr[this->instr_num]);
			this->reg_written[resO] = &this->All_Instr[this->instr_num];
			this->instr_num++;
		}
		else {
			ptr->chl.push_back(make_pair(x, ptr->cycle));
			x->indeg++;
		}

		if (self_change)
			this->up_AR_change[resA] = x;
	}

	//reg_info resA(AR);
	if (self_change == false) {
		//this->reg_read[res] = x;
		Instr* ptr = this->reg_written[resA];
		if (ptr == NULL) {
			Instr vptr;
			vptr.init();
			vptr.instr_str = (char*)malloc(1);
			vptr.instr_str[0] = '\0';
			vptr.chl.push_back(make_pair(x, vptr.cycle));
			x->indeg++;
			this->All_Instr[this->instr_num] = vptr;
			(this->container).push_back(&this->All_Instr[this->instr_num]);
			this->reg_written[resA] = &this->All_Instr[this->instr_num];
			this->instr_num++;
		}
		else {
			ptr->chl.push_back(make_pair(x, ptr->cycle));
			x->indeg++;
		}
	}
	else {
		Instr* ptr = this->reg_written[resA];
		if (ptr != NULL) {
			ptr->chl.push_back(make_pair(x, ptr->cycle));
			x->indeg++;
			for (i = 0; i < (ptr->chl).size(); i++) {
				Instr* child = (ptr->chl[i]).first;
				if (!strcmp(child->cond, AR) || !strcmp(child->input1, AR) || !strcmp(child->input2, AR) || !strcmp(child->input3, AR)) {
					if (x != child) {
						child->chl.push_back(make_pair(x, child->r_cycle - 1));
						x->indeg++;
					}
				}
			}
		}
		this->reg_written[resA] = x;
	}
	if (load_store) {
		// pack this Instr
		mem_info res(AR, OR, offset);
		this->mem_read[res] = x;
		for (map<mem_info, Instr*, mem_info_cmp>::iterator it = this->mem_written.begin(); it != this->mem_written.end();) {
			if (it->second != NULL && mem_rely(it->first, res)) {
				((it->second)->chl).push_back(make_pair(x, it->second->cycle));
				x->indeg++;
				//it++;
				(this->mem_written).erase(it++);
			}
			else {
				it++;
			}
		}
	}
}

void process_file(char* destination, char* source, void (Topograph::* Func)(FILE* fp)) {
	FILE* fpin, * fpout;
	fpin = fopen(source, "r");
	fpout = fopen(destination, "w");

	int st;// Abbreviation for sentence_type
	char sentence[256], buf[256];
	int para_flag;
	Instr* instr_x;

	new_topo:
	{
		Topograph topo;
		while (peek(fpin) != EOF) {
			fgets(sentence, 256, fpin);
			if (!strcmp(sentence, "\n")) continue;
			para_flag = get_first_word(buf, sentence);
			st = sentence_type(buf, sentence);
			//printf("type: %d \t %s\n\n", st, sentence);
			if (st) {
				// fprintf(fpout, "%s", sentence);
				if (st == 1) {
					(topo.*Func)(fpout);
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
			if (!para_flag) topo.Timer++;
			if (!strcmp(buf, "SNOP")) topo.Timer += instr_x->input1[0] - '0' - 1;
			//printf("%s \t type: %d \tfunc_unit:%s   \tcycle:%d \tcond:%s \tinput1:%s \tinput2:%s \tinput3:%s \toutput1:%s \tr_cycle:%d \tw_cycle:%d\n\n",\
				instr_x->instr_str, st, instr_x->func_unit, instr_x->cycle, instr_x->cond, instr_x->input1, instr_x->input2, \
				instr_x->input3, instr_x->output1, instr_x->r_cycle, instr_x->w_cycle);
			topo.fresh_cd_buff();
			//topo.build_dependency(instr_x);
			topo.push_to_cd_buff(instr_x);

			//Instr* x;
			//printf("reg_written info:\n");
			//for (unordered_map<reg_info, Instr*, reg_info_cmp>::iterator it = (topo.reg_written).begin(); it != (topo.reg_written).end(); it++) {
			//	x = it->second;
			//	if (x == NULL) continue;
			//	printf("%s", x->instr_str);
			//}puts("");

		}
		(topo.*Func)(fpout);
	}



	fclose(fpin);
	fclose(fpout);
}