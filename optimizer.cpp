#include <iostream>
#include <cstring>
#include <unordered_map>
#include <string>
#include "analyser.h"
#include "Topo_relative.h"
using namespace std;

char reference_filename[] = ".\\instruction.csv";
char input_file[] = ".\\test.asm";
char output_file[] = ".\\test_pro.asm";
extern Refer_table refer_table;

struct Var_list {
    string name;
    Var_list* next;
};

Var_list* get_var(string& reg) {
	int pre_pos = 0, colon_pos = -1;
	Var_list* node = new Var_list;
	Var_list* head = node;
	while ((colon_pos = reg.find(':', pre_pos)) != reg.npos) {
		node->name = reg.substr(pre_pos, colon_pos - pre_pos);
		node->next = new Var_list;
		node = node->next;
		pre_pos = colon_pos + 1;
	}
	node->name = reg.substr(pre_pos, reg.length() - pre_pos);
	node->next = NULL;
	return head;
}

int main()
{
    //refer_table.load_table(reference_filename);
    //load_func_table();
    //process_file(output_file, input_file, &(Topograph::reschedule));
	string test = "-1234";
	cout << stoll(test) << endl;
    return 0;
}

