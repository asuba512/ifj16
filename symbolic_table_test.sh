#!/bin/bash

ifj16 = pwd
ifj16_symbolic_table_test = "`pwd`/tests/symbol_table_tests"
ifj16_symbolic_table_output = "`pwd`/tests/symbol_table_tests/files.output"

number_of_symbolic_table_tests = ls $ifj16_symbolic_table_test | grep test_symbol_table_.* | wc -l

for i in $number_of_symbolic_table_tests
do

	test_file  = PRINT_RIADOK i | awk -F "," '{print $1}' $ifj16_symbolic_table_output
	error_flag = PRINT_RIADOK i | awk -F "," '{print $2}' $ifj16_symbolic_table_output 

	make clean
	make sem_test
	./sem_test $test_file > outputfile

	#TODO PRINT_RIADOK i
	#DIFF

done

rm outputfile