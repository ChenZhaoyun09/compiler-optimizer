﻿#include <cstdio>
#include <cstring>
#include <unordered_map>
#include "analyser.h"
#include "Topo_relative.h"
using namespace std;

char reference_filename[] = "D:\\program\\instruction.csv";
char input_file[] = "D:\\program\\test.asm";
char output_file[] = "D:\\program\\test_pro.asm";

int main()
{
    refer_table.load_table(reference_filename);
    process_file(output_file, input_file);
    return 0;
}

