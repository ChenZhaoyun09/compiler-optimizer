#pragma once
#include <cstdio>
#include "analyser.h"
using namespace std;


static bool Instr_cmp(const Instr* x, const Instr* y) {
	return x->indeg < y->indeg;
}


void Topograph::reschedule(FILE *fp) {
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
			no = find(this->container.begin(), this->container.end(), (x->chl[ii]).first)-this->container.begin();
			// fprintf(fp, "%d ", no);
			fprintf(fp, "(%d, %d) ", no, x->chl[ii].second);
		}// fprintf(fp, "\n");
		fprintf(fp, "\n");
	}
	// fprintf(fp, "\n");
	fprintf(fp, "\n");
}