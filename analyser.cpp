#include <cstdio>
#include <vector>
#include <cstring>
#include <queue>
#include <unordered_map>
#include <map>
#include "analyser.h"
using namespace std;


Refer_table refer_table;


int findchar(char* source, char c) {
	for (int i = 0, len = strlen(source); i < len; i++)
		if (source[i] == c)
			return i;
	return -1;
}


int hash_str(const char* str) {
	unsigned int seed = 257;
	unsigned int res = 0;
	while (*str) {
		res = res * seed + *str;
		str++;
	}
	return (int)res;
}


int get_first_word(char* destination, char* source) {
	int i = 0, ii;
	int para_flag = 0;
	//for (i = 0; source[i] == '\t' || source[i] == '|' || source[i] == ' '; i++);
	while (1) {
		if (source[i] != '\t' && source[i] != ' ' && source[i] != '|') break;
		if (source[i] == '|') para_flag = 1;
		i++;
	}
	for (ii = 0; source[i] != ' ' && source[i] != '\t' && source[i] != '\n'; i++, ii++)
		destination[ii] = source[i];
	destination[ii] = '\0';
	return para_flag;
}


//0: instruction;	1: label;	3: others;
int sentence_type(char* str, char* sentence) {
	int flag = findchar(str, ':');
	if (str[0] == ';') {
		int pos = findchar(sentence, 'c');
		if (pos != -1) return !strncmp(sentence + pos, "condjump", 8) ? 1 : 3;
	}
	else if (str[0] == '.' || str[0] == '\0') {
		if (flag != -1) return 1;
		else return !strncmp(sentence + 1, "size", 4) ? 1 : 3;
	}
	else {
		return flag != -1 ? 1 : 0;
	}
}


void Refer_table::load_table(char* filename) {
	int i, ii;
	char instr_info[256], buf[16];
	Refer_unit x;
	FILE* fp = fopen(filename, "r");


	fscanf(fp, "%s", instr_info);
	while (fscanf(fp, "%s", instr_info) != EOF) {
		for (ii = i = 0; instr_info[i] != ','; i++, ii++)
			x.name[ii] = instr_info[i];
		x.name[ii] = '\0';
		for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
			x.func_unit[ii] = instr_info[i];
		x.func_unit[ii] = '\0';
		for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
			buf[ii] = instr_info[i];
		buf[ii] = '\0';
		x.cycle = atoi(buf);
		for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
			x.input[0][ii] = instr_info[i];
		x.input[0][ii] = '\0';
		for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
			x.input[1][ii] = instr_info[i];
		x.input[1][ii] = '\0';
		for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
			x.input[2][ii] = instr_info[i];
		x.input[2][ii] = '\0';
		for (ii = 0, ++i; instr_info[i] != ','; i++, ii++)
			x.output[0][ii] = instr_info[i];
		x.output[0][ii] = '\0';
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

	fclose(fp);
}

Refer_unit Refer_table::get_refer_unit(char* str) {
	for (int i = 0; i < table.size(); i++)
		if (!strcmp(str, table[i].name))
			return table[i];
}


Instr* Topograph::generate_Instr(char* sentence) {
	Instr res;
	res.init();
	res.instr_str = (char*)malloc(strlen(sentence) * sizeof(char) + 4);
	strcpy(res.instr_str, sentence);


	int i, ii;
	// condition register
	for (i = 0; sentence[i] == '\t' || sentence[i] == '|' || sentence[i] == ' '; i++);
	if (sentence[i] == '[') {
		if (sentence[++i] == '!') i++;
		for (ii = 0; sentence[i] != ']'; ii++, i++)
			res.cond[ii] = sentence[i];
		res.cond[ii] = '\0';
		i++;
	}


	// func unit
	while (sentence[i] == '\t' || sentence[i] == ' ' || sentence[i] == ',') i++;
	for (ii = 0; sentence[i] != ' ' && sentence[i] != '\t' && sentence[i] != '\n'; i++, ii++)
		res.instr_name[ii] = sentence[i];
	res.instr_name[ii] = '\0';
	if ((ii = findchar(res.instr_name, '.')) != -1) res.instr_name[ii] = '\0';




	Refer_unit refer_unit = refer_table.get_refer_unit(res.instr_name);
	strcpy(res.func_unit, refer_unit.func_unit);


	if (strcmp(refer_unit.input[0], "")) {
		while (sentence[i] == '\t' || sentence[i] == ' ' || sentence[i] == ',') i++;
		for (ii = 0; sentence[i] != ',' && sentence[i] != '\n' && sentence[i] != '\t' && sentence[i] != ' '; i++, ii++)
			res.input1[ii] = sentence[i];
		res.input1[ii] = '\0';
		int c_flag = findchar(refer_unit.input[0], ':');
		if (c_flag != -1) {
			char c = refer_unit.input[0][c_flag + 1];
			int res_flag = findchar(res.input1, ':');
			if (res_flag == -1) {
				if (res.input1[0] == 'R' || (res.input1[0] == 'V' && res.input1[1] == 'R')) {
					res.input1[ii++] = ':';
					res.input1[ii++] = c;
					res.input1[ii++] = '\0';
				}
			}
			else {
				int tmp_i = 0;
				for (ii = res_flag + 1; ii < strlen(res.input1); ii++) {
					res.input1[tmp_i++] = res.input1[ii];
				}
				res.input1[tmp_i++] = ':';
				res.input1[tmp_i++] = c;
				res.input1[tmp_i++] = '\0';
			}
		}
	}


	if (strcmp(refer_unit.input[1], "")) {
		while (sentence[i] == '\t' || sentence[i] == ' ' || sentence[i] == ',') i++;
		//for (; sentence[i] != ','; i++);
		for (ii = 0; sentence[i] != ',' && sentence[i] != '\n' && sentence[i] != '\t' && sentence[i] != ' '; i++, ii++)
			res.input2[ii] = sentence[i];
		res.input2[ii] = '\0';
		int c_flag = findchar(refer_unit.input[1], ':');
		if (c_flag != -1) {
			char c = refer_unit.input[1][c_flag + 1];
			int res_flag = findchar(res.input2, ':');
			if (res_flag == -1) {
				if (res.input2[0] == 'R' || (res.input2[0] == 'V' && res.input2[1] == 'R')) {
					res.input2[ii++] = ':';
					res.input2[ii++] = c;
					res.input2[ii++] = '\0';
				}
			}
			else {
				int tmp_i = 0;
				for (ii = res_flag + 1; ii < strlen(res.input2); ii++) {
					res.input2[tmp_i++] = res.input2[ii];
				}
				res.input2[tmp_i++] = ':';
				res.input2[tmp_i++] = c;
				res.input2[tmp_i++] = '\0';
			}
		}

	}


	if (strcmp(refer_unit.input[2], "")) {
		while (sentence[i] == '\t' || sentence[i] == ' ' || sentence[i] == ',') i++;
		//for (; sentence[i] != ','; i++);
		for (ii = 0; sentence[i] != ',' && sentence[i] != '\n' && sentence[i] != '\t' && sentence[i] != ' '; i++, ii++)
			res.input3[ii] = sentence[i];
		res.input3[ii] = '\0';
		int c_flag = findchar(refer_unit.input[2], ':');
		if (c_flag != -1) {
			char c = refer_unit.input[2][c_flag + 1];
			int res_flag = findchar(res.input3, ':');
			if (res_flag == -1) {
				if (res.input3[0] == 'R' || (res.input3[0] == 'V' && res.input3[1] == 'R')) {
					res.input3[ii++] = ':';
					res.input3[ii++] = c;
					res.input3[ii++] = '\0';
				}
			}
			else {
				int tmp_i = 0;
				for (ii = res_flag + 1; ii < strlen(res.input3); ii++) {
					res.input3[tmp_i++] = res.input3[ii];
				}
				res.input3[tmp_i++] = ':';
				res.input3[tmp_i++] = c;
				res.input3[tmp_i++] = '\0';
			}
		}
	}


	if (strcmp(refer_unit.output[0], "")) {
		while (sentence[i] == '\t' || sentence[i] == ' ' || sentence[i] == ',') i++;
		//for (; sentence[i] != ','; i++);
		for (ii = 0; sentence[i] != ',' && sentence[i] != '\n' && sentence[i] != '\t' && sentence[i] != ' '; i++, ii++)
			res.output1[ii] = sentence[i];
		res.output1[ii] = '\0';
		int c_flag = findchar(refer_unit.output[0], ':');
		if (c_flag != -1) {
			char c = refer_unit.output[0][c_flag + 1];
			int res_flag = findchar(res.output1, ':');
			if (res_flag == -1) {
				if (res.output1[0] == 'R' || (res.output1[0] == 'V' && res.output1[1] == 'R')) {
					res.output1[ii++] = ':';
					res.output1[ii++] = c;
					res.output1[ii++] = '\0';
				}
			}
			else {
				int tmp_i = 0;
				for (ii = res_flag + 1; ii < strlen(res.output1); ii++) {
					res.output1[tmp_i++] = res.output1[ii];
				}
				res.output1[tmp_i++] = ':';
				res.output1[tmp_i++] = c;
				res.output1[tmp_i++] = '\0';
			}
		}

	}




	res.cycle = refer_unit.cycle;
	if (!strcmp(res.instr_name, "SADDA") || !strcmp(res.instr_name, "SSUBA")) {
		int k = 0;
		char output[10];
		int num = 0;
		while (res.output1[k] < '0' || res.output1[k] > '9') k++;
		for (ii = 0; res.output1[k] != '\0'; k++, ii++)
			output[ii] = res.output1[k];
		output[ii] = '\0';
		num = atoi(output);
		if (num > 7) res.cycle--;
	}

	res.r_cycle = refer_unit.r_cycle;
	res.w_cycle = refer_unit.w_cycle;

	All_Instr[instr_num] = res;

	return &All_Instr[instr_num];
}