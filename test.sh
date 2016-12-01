#!/bin/sh

############################################################
# ON  switch == 0
# OFF swtich != 0


TOTAL_ERROR_COUNT=0
switch_make=0

# "valgrind ./program file"
switch_VALGRIND=1

switch_GOOD_dir_1=1
GOOD_dir_1="`pwd`/tests/codes_without_error"

switch_BAD_dir_1=1
BAD_dir_1="`pwd`/tests/semantic_tests"

switch_BAD_dir_2=1
BAD_dir_2="`pwd`/tests/parser_tests"

# SCANNER TESTS - testing $?==1, folder include source codes instead of expected output
switch_SCANNER_DIR_3=1
SCANNER_DIR_3="`pwd`/tests/scanner_tests_3"

switch_EXITCODE_TESTS=1
EXITCODE_TESTS="`pwd`/tests/exitcode_tests"

# expecting that the code is without error and the existatus of "./ifj FILE" is 0
switch_MANUAL_CODES=1
MANUAL_CODES="`pwd`/tests/codes_from_manual"

switch_WORKING_CODES=1
WORKING_CODES="`pwd`/tests/working_source_codes"

switch_STDOUT_DIR=1
STDOUT_DIR="`pwd`/tests/stdout_tests"

	# SCANNER TESTS (the first one) are for nothing because the behaviour was changed with FQID
	switch_SCANNER_DIR=1 # keep your dirty hands off
	SCANNER_DIR="`pwd`/tests/scanner_tests"

# SCANNER TESTS - the new folder
switch_SCANNER_DIR_2=1
SCANNER_DIR_2="`pwd`/tests/scanner_tests_2"

switch_SYMBOLIC_TABLE=1
SYMBOLIC_TABLE="`pwd`/tests/symbol_table_tests"
############################################################

if [ $# -eq 0 ]; then
	printf "Unexpected arguments. \"$0\" aborted!\n"
	exit 1
fi

if [ $# -gt 1 ]; then
	printf "Unexpected arguments. \"$0\" aborted!\n"
	exit 1
fi

if [ $# -eq 1 ]; then
	c=$(echo $1 | cut -c1)
	if [ $c != "-" ]; then
		printf "Unexpected arguments. \"$0\" aborted!\n"
		exit 1
	fi
fi

while getopts "tcCoOhHgGsSpPaAeE" opt; do
	case "$opt" in
		t) printf "== TESTS ==\n"
				switch_make=1
				switch_GOOD_dir_1=0
				switch_BAD_dir_1=0
				switch_BAD_dir_2=0
				switch_SCANNER_DIR_3=0
				switch_EXITCODE_TESTS=0
				switch_MANUAL_CODES=1
				switch_WORKING_CODES=0
				switch_STDOUT_DIR=0
				switch_SCANNER_DIR=1
				switch_SCANNER_DIR_2=1
				switch_SYMBOLIC_TABLE=1
			;;
		c)  printf "== WORKING SOURCE CODES ==\n"
				switch_WORKING_CODES=0
			;;
		C)  printf "== WORKING SOURCE CODES with VALGRIND ==\n"
				switch_VALGRIND=0
				switch_WORKING_CODES=0
			;; 
		o)  printf "== STDOUT .OUTPUT COMPARISON ==\n"
				switch_STDOUT_DIR=0
			;;
		O)  printf "== STDOUT .OUTPUT COMPARISON with VALGRIND ==\n"
				switch_VALGRIND=0
				switch_STDOUT_DIR=0
			;;        		
		g)  printf "== TESTS WITHOUT ERROR ==\n"
				switch_GOOD_dir_1=0
			;;
		G)  printf "== TESTS WITHOUT ERROR with VALGRIND ==\n"
				switch_VALGRIND=0
				switch_GOOD_dir_1=0
			;;          
		e)  printf "== EXITCODE ==\n"
				switch_EXITCODE_TESTS=0
			;;
		E)  printf "== EXITCODE with VALGRIND ==\n"
				switch_VALGRIND=0
				switch_EXITCODE_TESTS=0
			;;
		s)  printf "== SCANNER ==\n"
				switch_SCANNER_DIR_3=0
				switch_SCANNER_DIR=1
				switch_SCANNER_DIR_2=0
			;;
		S)  printf "== SCANNER with VALGRIND ==\n"
				switch_VALGRIND=0
				switch_SCANNER_DIR_3=0
				switch_SCANNER_DIR=1
				switch_SCANNER_DIR_2=0
			;;
		p)  printf "== PARSER ==\n"
				switch_BAD_dir_2=0
			;;
		P)  printf "== PARSER with VALGRIND ==\n"
				switch_VALGRIND=0
				switch_BAD_dir_2=0
			;;
		a)  printf "== ALL ==\n"
				switch_GOOD_dir_1=0
				switch_BAD_dir_1=0
				switch_BAD_dir_2=0
				switch_SCANNER_DIR_3=0
				switch_EXITCODE_TESTS=0
				switch_MANUAL_CODES=0
				switch_WORKING_CODES=0
				switch_STDOUT_DIR=0
				switch_SCANNER_DIR=1
				switch_SCANNER_DIR_2=0
				switch_SYMBOLIC_TABLE=1
			;;
		A)  printf "== ALL with VALGRIND ==\n"
				switch_VALGRIND=0
				switch_GOOD_dir_1=0
				switch_BAD_dir_1=0
				switch_BAD_dir_2=0
				switch_SCANNER_DIR_3=0
				switch_EXITCODE_TESTS=0
				switch_MANUAL_CODES=0
				switch_WORKING_CODES=0
				switch_STDOUT_DIR=0
				switch_SCANNER_DIR=1
				switch_SCANNER_DIR_2=0
				switch_SYMBOLIC_TABLE=1
			;;
		h)  printf "== HELP ==\n"
			printf "  -a    all tests without valgrind\n"
			printf "  -A    all tests with valgrind\n"
			printf "  -s    scanner tests without valgrind\n"
			printf "  -S    scanner tests with valgrind\n"
			printf "  -p    parser tests without valgrind\n"
			printf "  -P    parser tests with valgrind\n"
			printf "  -e    exitcode tests without valgrind\n"
			printf "  -E    exitcode tests with valgrind\n"
			printf "  -g    without error tests without valgrind\n"
			printf "  -G    without error tests with valgrind\n"
			printf "  -o    stdout comparison tests without valgrind\n"
			printf "  -O    stdout comparison tests with valgrind\n"
			printf "  -c    working source codes tests without valgrind\n"
			printf "  -C    working source codes tests with valgrind\n"
			printf "  -h    display help\n"
			printf "  -H    display help\n"
			exit 0
			;;
		H)  printf "== HELP ==\n"
			printf "  -a    all tests without valgrind\n"
			printf "  -A    all tests with valgrind\n"
			printf "  -s    scanner tests without valgrind\n"
			printf "  -S    scanner tests with valgrind\n"
			printf "  -p    parser tests without valgrind\n"
			printf "  -P    parser tests with valgrind\n"
			printf "  -e    exitcode tests without valgrind\n"
			printf "  -E    exitcode tests with valgrind\n"
			printf "  -g    without error tests without valgrind\n"
			printf "  -G    without error tests with valgrind\n"
			printf "  -o    stdout comparison tests without valgrind\n"
			printf "  -O    stdout comparison tests with valgrind\n"
			printf "  -c    working source codes tests without valgrind\n"
			printf "  -C    working source codes tests with valgrind\n"
			printf "  -h    display help\n"
			printf "  -H    display help\n"
			exit 0
			;;
	   '?') printf "Unexpected arguments. \"$0\" aborted!\n"
			exit 1
			;;
	esac
done

############################################################
if [ $switch_make -eq 0 ]; then
	if [ $switch_GOOD_dir_1     -eq 0 ] || [ $switch_BAD_dir_1     -eq 0 ] || [ $switch_BAD_dir_2     -eq 0 ] ||
	   [ $switch_EXITCODE_TESTS -eq 0 ] || [ $switch_MANUAL_CODES  -eq 0 ] || [ $switch_SCANNER_DIR_3 -eq 0 ] ||
	   [ $switch_STDOUT_DIR     -eq 0 ] || [ $switch_WORKING_CODES -eq 0 ]; then
		printf "\n\n=== IFJ BUILD ===\n"
		printf "=== make clean ===\n"
		make clean
		printf "=== make ===\n"
		make
	fi
fi
############################################################
if [ $switch_GOOD_dir_1 -eq 0 ]; then

	dir=$(echo $GOOD_dir_1 | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS WITHOUT ERROR ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF ("$""?" == 0) AND (STDERR is EMPTY) THEN TEST PASSED ===\n"
	printf "==\n"

	counter=0
	for i in `ls $GOOD_dir_1`
	do
		counter=$(expr $counter + 1)
		printf "= $counter.)\t$i \n"

		./ifj $GOOD_dir_1/$i >/dev/null 2>stderr_file

		exitvalue=$?
		stderr_check=$(cat stderr_file | wc -l)

		#DEBUG
		#printf "\t$exitvalue "$""?"\n"
		#printf "\t$stderr_check stderr\n"

		if [ $exitvalue -ne 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\t"$""?" = $exitvalue\n"
		fi
		if [ $stderr_check -ne 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\tSTDERR is NOT empty\n"
		fi

		if [ $switch_VALGRIND -eq 0 ]; then
			valgrind ./ifj $GOOD_dir_1/$i >valgrind_file 2>&1

			summary=$(cat valgrind_file | grep ".*ERROR SUMMARY:.*")
			#echo $summary
			memory_leaks=$(echo $summary| grep ".*ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0).*" | wc -l)
			#echo $memory_leaks

			if [ $memory_leaks -ne 1 ]; then
				TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
				printf "\t\t$summary\n"
			fi
		fi

	done

	if [ $switch_VALGRIND -eq 0 ]; then
		rm valgrind_file
	fi
	rm stderr_file
fi

############################################################
if [ $switch_BAD_dir_1 -eq 0 ]; then

	dir=$(echo $BAD_dir_1 | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS WITH ERROR ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF ("$""?" == exitvalue in .output) AND (STDERR is NOT EMPTY) THEN TEST PASSED ===\n"
	printf "==\n"

	counter=0
	for i in `ls $BAD_dir_1`
	do
		# file name ends with "...output" then skip this file
		i_cond=$(echo $i | grep ^.*output$ | wc -l)
		if [ $i_cond -eq 1 ]; then
			continue
		fi

		counter=$(expr $counter + 1)
		printf "= $counter.)\t$i \n"

		./ifj $BAD_dir_1/$i >/dev/null 2>stderr_file

		exitvalue=$?
		stderr_check=$(cat stderr_file | wc -l)

		#DEBUG
		#printf "\t$exitvalue "$""?"\n"
		#printf "\t$stderr_check stderr\n"

		file_value=$(cat "$BAD_dir_1/$i.output")

		if [ $exitvalue -ne $file_value ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\t"$""?" = $exitvalue, expecting $file_value\n"
		fi
		if [ $stderr_check -eq 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\tSTDERR is empty\n"
		fi

		if [ $switch_VALGRIND -eq 0 ]; then
			valgrind ./ifj $BAD_dir_1/$i >valgrind_file 2>&1

			summary=$(cat valgrind_file | grep ".*ERROR SUMMARY:.*")
			#echo $summary
			memory_leaks=$(echo $summary| grep ".*ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0).*" | wc -l)
			#echo $memory_leaks

			if [ $memory_leaks -ne 1 ]; then
				TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
				printf "\t\t$summary\n"
			fi
		fi

	done

	if [ $switch_VALGRIND -eq 0 ]; then
		rm valgrind_file
	fi
	rm stderr_file
fi

############################################################
if [ $switch_BAD_dir_2 -eq 0 ]; then

	dir=$(echo $BAD_dir_2 | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS WITH ERROR ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF ("$""?" != 2) AND (STDERR is NOT EMPTY) THEN TEST PASSED ===\n"
	printf "==\n"


	counter=0
	for i in `ls $BAD_dir_2`
	do

		# TODO check THE EXITVALUE WITH SAVED VALUE IN FILE WITH .exitvalue
		# THIS WILL IGNORE .exitvalue files TODO write the files !
		#i_cond=$(echo $i | grep ^.*exitvalue$ | wc -l) # file name ends with "...exitvalue" then skip this file
		#if [ $i_cond -eq 1 ]; then
		#   continue
		#fi

		counter=$(expr $counter + 1)
		printf "= $counter.)\t$i \n"

		./ifj $BAD_dir_2/$i >/dev/null 2>stderr_file

		exitvalue=$?
		stderr_check=$(cat stderr_file | wc -l)

		#DEBUG
		#printf "\t$exitvalue "$""?"\n"
		#printf "\t$stderr_check stderr\n"

		if [ $exitvalue -ne 2 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\t"$""?" = $exitvalue\n"
		fi
		if [ $stderr_check -eq 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\tSTDERR is empty\n"
		fi

		if [ $switch_VALGRIND -eq 0 ]; then
			valgrind ./ifj $BAD_dir_2/$i >valgrind_file 2>&1

			summary=$(cat valgrind_file | grep ".*ERROR SUMMARY:.*")
			#echo $summary
			memory_leaks=$(echo $summary| grep ".*ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0).*" | wc -l)
			#echo $memory_leaks

			if [ $memory_leaks -ne 1 ]; then
				TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
				printf "\t\t$summary\n"
			fi
		fi

	done

	if [ $switch_VALGRIND -eq 0 ]; then
		rm valgrind_file
	fi
	rm stderr_file
fi

############################################################
if [ $switch_SCANNER_DIR_3 -eq 0 ]; then

	dir=$(echo $SCANNER_DIR_3 | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS WITH ERROR ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF ("$""?" != 1) AND (STDERR is NOT EMPTY) THEN TEST PASSED ===\n"
	printf "==\n"


	counter=0
	for i in `ls $SCANNER_DIR_3`
	do

		# TODO check THE EXITVALUE WITH SAVED VALUE IN FILE WITH .exitvalue
		# THIS WILL IGNORE .exitvalue files TODO write the files !
		#i_cond=$(echo $i | grep ^.*exitvalue$ | wc -l) # file name ends with "...exitvalue" then skip this file
		#if [ $i_cond -eq 1 ]; then
		#   continue
		#fi

		counter=$(expr $counter + 1)
		printf "= $counter.)\t$i \n"

		./ifj $SCANNER_DIR_3/$i >/dev/null 2>stderr_file

		exitvalue=$?
		stderr_check=$(cat stderr_file | wc -l)

		#DEBUG
		#printf "\t$exitvalue "$""?"\n"
		#printf "\t$stderr_check stderr\n"

		if [ $exitvalue -ne 1 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\t"$""?" = $exitvalue\n"
		fi
		if [ $stderr_check -eq 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\tSTDERR is empty\n"
		fi

		if [ $switch_VALGRIND -eq 0 ]; then
			valgrind ./ifj $SCANNER_DIR_3/$i >valgrind_file 2>&1

			summary=$(cat valgrind_file | grep ".*ERROR SUMMARY:.*")
			#echo $summary
			memory_leaks=$(echo $summary| grep ".*ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0).*" | wc -l)
			#echo $memory_leaks

			if [ $memory_leaks -ne 1 ]; then
				TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
				printf "\t\t$summary\n"
			fi
		fi

	done

	if [ $switch_VALGRIND -eq 0 ]; then
		rm valgrind_file
	fi
	rm stderr_file
fi

############################################################
if [ $switch_EXITCODE_TESTS -eq 0 ]; then

	dir=$(echo $EXITCODE_TESTS | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS WITH ERROR ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF ("$""?" == exitvalue in .output) AND (STDERR is NOT EMPTY) THEN TEST PASSED ===\n"
	printf "==\n"

	counter=0
	for i in `ls $EXITCODE_TESTS`
	do
		# file name ends with "...output" then skip this file
		i_cond=$(echo $i | grep ^.*output$ | wc -l)
		if [ $i_cond -eq 1 ]; then
			continue
		fi

		counter=$(expr $counter + 1)
		printf "= $counter.)\t$i \n"

		./ifj $EXITCODE_TESTS/$i >/dev/null 2>stderr_file

		exitvalue=$?
		stderr_check=$(cat stderr_file | wc -l)

		#DEBUG
		#printf "\t$exitvalue "$""?"\n"
		#printf "\t$stderr_check stderr\n"

		file_value=$(cat "$EXITCODE_TESTS/$i.output")

		if [ $exitvalue -ne $file_value ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\t"$""?" = $exitvalue, expecting $file_value\n"
		fi
		if [ $stderr_check -eq 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\tSTDERR is empty\n"
		fi

		if [ $switch_VALGRIND -eq 0 ]; then
			valgrind ./ifj $EXITCODE_TESTS/$i >valgrind_file 2>&1

			summary=$(cat valgrind_file | grep ".*ERROR SUMMARY:.*")
			#echo $summary
			memory_leaks=$(echo $summary| grep ".*ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0).*" | wc -l)
			#echo $memory_leaks

			if [ $memory_leaks -ne 1 ]; then
				TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
				printf "\t\t$summary\n"
			fi
		fi

	done

	if [ $switch_VALGRIND -eq 0 ]; then
		rm valgrind_file
	fi
	rm stderr_file
fi

############################################################
if [ $switch_MANUAL_CODES -eq 0 ]; then

	dir=$(echo $MANUAL_CODES | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS MANUAL CODES ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF ("$""?" == 0) AND (STDERR is EMPTY) THEN TEST PASSED ===\n"
	printf "==\n"

	> empty_file

	counter=0
	for i in `ls $MANUAL_CODES`
	do
		counter=$(expr $counter + 1)
		printf "= $counter.)\t$i \n"

		./ifj $MANUAL_CODES/$i 2>stderr_file

		exitvalue=$?

		diff stderr_file empty_file > /dev/null
		exitvalue_diff=$?

		if [ $exitvalue -ne 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\t"$""?" = $exitvalue\n"
		fi
		if [ $exitvalue_diff -ne 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\tSTDERR is NOT empty\n"
		fi

	done

	rm stderr_file
	rm empty_file
fi

############################################################
if [ $switch_WORKING_CODES -eq 0 ]; then

	dir=$(echo $WORKING_CODES | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS WORKING CODES ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF ("$""?" == 0) AND (STDERR is EMPTY) AND (STDOUT == responding file.output) THEN TEST PASSED ===\n"
	printf "==\n"

	> empty_file

	counter=0
	for i in `ls $WORKING_CODES`
	do
		# file name ends with "...output" then skip this file
		i_cond=$(echo $i | grep ^.*output$ | wc -l)
		if [ $i_cond -eq 1 ]; then
			continue
		fi

		counter=$(expr $counter + 1)
		printf "= $counter.)\t$i \n"

		./ifj $WORKING_CODES/$i >stdout_file 2>stderr_file

		exitvalue=$?

		diff stderr_file empty_file > /dev/null
		exitvalue_diff_stderr=$?

		diff stdout_file "$WORKING_CODES/$i.output" >/dev/null
		exitvalue_diff_stdout=$?

		if [ $exitvalue -ne 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\t"$""?" = $exitvalue\n"
		fi
		if [ $exitvalue_diff_stderr -ne 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\tSTDERR is NOT empty\n"
		fi

		if [ $exitvalue_diff_stdout -ne 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\tDIFF "$""?" = 1 (different .output <-> stdout)\n"
		fi

		if [ $switch_VALGRIND -eq 0 ]; then
			valgrind ./ifj $WORKING_CODES/$i >valgrind_file 2>&1

			summary=$(cat valgrind_file | grep ".*ERROR SUMMARY:.*")
			#echo $summary
			memory_leaks=$(echo $summary| grep ".*ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0).*" | wc -l)
			#echo $memory_leaks

			if [ $memory_leaks -ne 1 ]; then
				TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
				printf "\t\t$summary\n"
			fi
		fi

	done

	if [ $switch_VALGRIND -eq 0 ]; then
		rm valgrind_file
	fi
	rm stdout_file
	rm stderr_file
	rm empty_file
fi

############################################################
if [ $switch_STDOUT_DIR -eq 0 ]; then

	dir=$(echo $STDOUT_DIR | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS WITHOUT ERROR ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF ("$""?" == 0) AND (STDERR is EMPTY) AND (STDOUT == responding file.output) THEN TEST PASSED ===\n"
	printf "==\n"

	counter=0
	for i in `ls $STDOUT_DIR`
	do
		# file name ends with "...output" then skip this file
		i_cond=$(echo $i | grep ^.*output$ | wc -l)
		if [ $i_cond -eq 1 ]; then
			continue
		fi

		counter=$(expr $counter + 1)
		printf "= $counter.)\t$i \n"

		./ifj $STDOUT_DIR/$i >stdout_file 2>stderr_file

		exitvalue=$?
		stderr_check=$(cat stderr_file | wc -l)

		#DEBUG
		#printf "\t$exitvalue "$""?"\n"
		#printf "\t$stderr_check stderr\n"

		if [ $exitvalue -ne 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\t"$""?" = $exitvalue\n"
		fi
		if [ $stderr_check -ne 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\t\tSTDERR is NOT empty\n"
		fi

		diff stdout_file "$STDOUT_DIR/$i.output" >/dev/null
		diff_exitvalue=$?

		if [ $diff_exitvalue -ne 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\tDIFF "$""?" = 1 (different .output <-> stdout)\n"
		fi


		if [ $switch_VALGRIND -eq 0 ]; then
			valgrind ./ifj $STDOUT_DIR/$i >valgrind_file 2>&1

			summary=$(cat valgrind_file | grep ".*ERROR SUMMARY:.*")
			#echo $summary
			memory_leaks=$(echo $summary| grep ".*ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0).*" | wc -l)
			#echo $memory_leaks

			if [ $memory_leaks -ne 1 ]; then
				TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
				printf "\t\t$summary\n"
			fi
		fi

	done

	if [ $switch_VALGRIND -eq 0 ]; then
		rm valgrind_file
	fi
	rm stdout_file
	rm stderr_file
fi

############################################################
if [ $switch_make -eq 0 ]; then
	if [ $switch_SCANNER_DIR -eq 0 ] || [ $switch_SCANNER_DIR_2 -eq 0 ] ; then
		printf "\n\n=== IFJ SCANNER BUILD ===\n"
		printf "=== make clean ===\n"
		make clean
		printf "=== make scnr_test ===\n"
		make scnr_test
	fi
fi

############################################################
# SCANNER TESTS (the first one) are for nothing because the behaviour was changed with FQID
if [ $switch_SCANNER_DIR -eq 0 ]; then

	dir=$(echo $SCANNER_DIR | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS DIFF ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF (stdout == responding file.output) THEN TEST PASSED ===\n"
	printf "==\n"

	counter=0
	for i in `ls $SCANNER_DIR`
	do
		# file name ends with "...output" then skip this file
		i_cond=$(echo $i | grep ^.*output$ | wc -l)
		if [ $i_cond -eq 1 ]; then
			continue
		fi

		counter=$(expr $counter + 1)
		printf "= $counter.)\t$i \n"

		./sc_test "$SCANNER_DIR/$i" >output_file

		diff output_file "$SCANNER_DIR/$i.output" >/dev/null
		exitvalue=$?

		if [ $exitvalue -ne 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\tDIFF "$""?" = 1 (different output)\n"
		fi

	done

	rm output_file
fi

############################################################
if [ $switch_SCANNER_DIR_2 -eq 0 ]; then

	dir=$(echo $SCANNER_DIR_2 | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS DIFF ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF (stdout == responding file.output) THEN TEST PASSED ===\n"
	printf "==\n"

	counter=0
	for i in `ls $SCANNER_DIR_2`
	do
		# file name ends with "...output" then skip this file
		i_cond=$(echo $i | grep ^.*output$ | wc -l)
		if [ $i_cond -eq 1 ]; then
			continue
		fi

		counter=$(expr $counter + 1)
		printf "= $counter.) $i \n"

		./sc_test "$SCANNER_DIR_2/$i" >output_file

		diff output_file "$SCANNER_DIR_2/$i.output" >/dev/null
		exitvalue=$?

		if [ $exitvalue -ne 0 ]; then
			TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
			printf "\tDIFF "$""?" = 1 (different output)\n"
		fi

	done

	rm output_file
fi

############################################################
if [ $switch_make -eq 0 ]; then
	if [ $switch_SYMBOLIC_TABLE -eq 0 ]; then
		printf "\n\n=== IFJ SYMBOLIC_TABLE BUILD ===\n"
		printf "=== make clean ===\n"
		make clean
		printf "=== make sem_test ===\n"
		make sem_test
	fi
fi

############################################################
if [ $switch_SYMBOLIC_TABLE -eq 0 ]; then

	dir=$(echo $SYMBOLIC_TABLE | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS SYMBOLIC TABLE ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF (stdout == responding file.output) THEN TEST PASSED ===\n"

	counter=0
	for i in `ls $SYMBOLIC_TABLE`
	do
		# file name ends with "...output" then skip this file
		i_cond=$(echo $i | grep ^.*output$ | wc -l)
		if [ $i_cond -eq 1 ]; then
			continue
		fi

		counter=$(expr $counter + 1)
		printf "= $counter.)\t$i \n"

		./sem_test "$SYMBOLIC_TABLE/$i" >/dev/null 2>stderr_file

		ok=$(cat $SYMBOLIC_TABLE/$i.output | head -1)

		if [ $ok = "=ERROR=" ]; then
			number_of_errors_file=$(cat $SYMBOLIC_TABLE/$i.output | head -2 | tail -1 | awk '{print $3}')
			number_of_errors_stderr=$(cat stderr_file | grep ^.*ERR.*$ | wc -l)
			if [ $number_of_errors_file -ne $number_of_errors_stderr ]; then
				TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
				printf "\t\tNumber of stderr errors  = $number_of_errors_stderr\n"
				printf "\t\tExpected count of errors = $number_of_errors_file\n"
			fi
		fi

		if [ $ok = "=OK=" ]; then
			number_of_errors_stderr=$(cat stderr_file | grep ^.*ERR.*$ | wc -l)
			if [ $number_of_errors_stderr -ne 0 ]; then
				TOTAL_ERROR_COUNT=$(expr $TOTAL_ERROR_COUNT + 1)
				printf "\t\tNumber of stderr errors  = $number_of_errors_stderr\n"
				printf "\t\tExpected count of errors = 0\n"
			fi
		fi

	done

	rm stderr_file
fi

############################################################
if [ $switch_make -eq 0 ]; then
	if [ $switch_GOOD_dir_1     -eq 0 ] || [ $switch_BAD_dir_1     -eq 0 ] || [ $switch_BAD_dir_2      -eq 0 ] ||
	   [ $switch_MANUAL_CODES   -eq 0 ] || [ $switch_SCANNER_DIR   -eq 0 ] || [ $switch_SCANNER_DIR_2  -eq 0 ] ||
	   [ $switch_SYMBOLIC_TABLE -eq 0 ] || [ $switch_SCANNER_DIR_3 -eq 0 ] || [ $switch_EXITCODE_TESTS -eq 0 ] ||
	   [ $switch_STDOUT_DIR     -eq 0 ] || [ $switch_WORKING_CODES -eq 0 ]; then
		printf "\n\n=== MAKE CLEAN ===\n"
		make clean
		printf "\n\n=== FINISH ===\n"
		printf "Total number of ERRORs = $TOTAL_ERROR_COUNT\n"
	else
		printf "=== TESTS ARE SWITCHED OFF ===\n=== FINISH ====\n"
	fi
else
	printf "\n\n=== FINISH ===\n"
	printf "Total number of ERRORs = $TOTAL_ERROR_COUNT\n"
fi