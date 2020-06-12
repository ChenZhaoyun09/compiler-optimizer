#include <cstdio>
#include <cstring>
#include <unordered_map>
#include "analyser.h"
#include "Topo_relative.h"
using namespace std;

char reference_filename[] = ".\\instruction.csv";
char input_file[] = ".\\test.asm";
char output_file[] = ".\\test_pro.asm";

int main()
{
    refer_table.load_table(reference_filename);
    load_func_table();
    process_file(output_file, input_file, &(Topograph::reschedule));
    return 0;
}

