#!/bin/sh
ifj16_dir=pwd

############################################################
# ON  switch == 0
# OFF swtich != 0

# valgrind ./program file
switch_VALGRIND=0

switch_GOOD_dir_1=0
GOOD_dir_1="`pwd`/tests/codes_without_error"

switch_BAD_dir_1=0
BAD_dir_1="`pwd`/tests/semantic_tests"

switch_BAD_dir_2=0
BAD_dir_2="`pwd`/tests/parser_tests"

# SCANNER TESTS - testing $?==1, folder include source codes instead of expected output
switch_SCANNER_DIR_3=1
SCANNER_DIR_3="`pwd`/tests/scanner_tests_3"

# expecting that the code is without error and the existatus of "./ifj FILE" is 0
switch_MANUAL_CODES=1
MANUAL_CODES="`pwd`/tests/codes_from_manual"

	# SCANNER TESTS (the first one) are for nothing because the behaviour was changed with FQID
	switch_SCANNER_DIR=1 # keep your dirty hands off
	SCANNER_DIR="`pwd`/tests/scanner_tests"

# SCANNER TESTS - the new folder
switch_SCANNER_DIR_2=1
SCANNER_DIR_2="`pwd`/tests/scanner_tests_2"

switch_SYMBOLIC_TABLE=1
SYMBOLIC_TABLE="`pwd`/tests/symbol_table_tests"



############################################################
if [ $switch_GOOD_dir_1 -eq 0 ]   || [ $switch_BAD_dir_1 -eq 0 ] || [ $switch_BAD_dir_2 -eq 0 ] ||
   [ $switch_MANUAL_CODES -eq 0 ] || [ $switch_SCANNER_DIR_3 -eq 0]; then
	printf "\n\n=== IFJ BUILD ===\n"
	printf "=== make clean ===\n"
	make clean
	printf "=== make ===\n"
	make
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
			printf "\t\t"$""?" = $exitvalue\n"
		fi
		if [ $stderr_check -ne 0 ]; then
			printf "\t\tSTDERR is NOT empty\n"
		fi

		if [ $switch_VALGRIND -eq 0 ]; then
			valgrind ./ifj $GOOD_dir_1/$i >valgrind_file 2>&1

			summary=$(cat valgrind_file | grep ".*ERROR SUMMARY:.*")
			#echo $summary
			memory_leaks=$(echo $summary| grep ".*ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0).*" | wc -l)
			#echo $memory_leaks

			if [ $memory_leaks -ne 1 ]; then
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
			printf "\t\t"$""?" = $exitvalue, expecting $file_value\n"
		fi
		if [ $stderr_check -eq 0 ]; then
			printf "\t\tSTDERR is empty\n"
		fi

		if [ $switch_VALGRIND -eq 0 ]; then
			valgrind ./ifj $BAD_dir_1/$i >valgrind_file 2>&1

			summary=$(cat valgrind_file | grep ".*ERROR SUMMARY:.*")
			#echo $summary
			memory_leaks=$(echo $summary| grep ".*ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0).*" | wc -l)
			#echo $memory_leaks

			if [ $memory_leaks -ne 1 ]; then
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
			printf "\t\t"$""?" = $exitvalue\n"
		fi
		if [ $stderr_check -eq 0 ]; then
			printf "\t\tSTDERR is empty\n"
		fi

		if [ $switch_VALGRIND -eq 0 ]; then
			valgrind ./ifj $BAD_dir_2/$i >valgrind_file 2>&1

			summary=$(cat valgrind_file | grep ".*ERROR SUMMARY:.*")
			#echo $summary
			memory_leaks=$(echo $summary| grep ".*ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0).*" | wc -l)
			#echo $memory_leaks

			if [ $memory_leaks -ne 1 ]; then
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
			printf "\t\t"$""?" = $exitvalue\n"
		fi
		if [ $stderr_check -eq 0 ]; then
			printf "\t\tSTDERR is empty\n"
		fi

		if [ $switch_VALGRIND -eq 0 ]; then
			valgrind ./ifj $SCANNER_DIR_3/$i >valgrind_file 2>&1

			summary=$(cat valgrind_file | grep ".*ERROR SUMMARY:.*")
			#echo $summary
			memory_leaks=$(echo $summary| grep ".*ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0).*" | wc -l)
			#echo $memory_leaks

			if [ $memory_leaks -ne 1 ]; then
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
			printf "\t\t"$""?" = $exitvalue\n"
		fi
		if [ $exitvalue_diff -ne 0 ]; then
			printf "\t\tSTDERR is NOT empty\n"
		fi

	done

	rm stderr_file
	rm empty_file
fi

############################################################
if [ $switch_SCANNER_DIR -eq 0 ] || [ $switch_SCANNER_DIR_2 -eq 0 ] ; then
	printf "\n\n=== IFJ SCANNER BUILD ===\n"
	printf "=== make clean ===\n"
	make clean
	printf "=== make test ===\n"
	make test
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
			printf "\tDIFF "$""?" = 1 (different output)\n"
		fi

	done

	rm output_file
fi

############################################################
if [ $switch_SYMBOLIC_TABLE -eq 0 ]; then
	printf "\n\n=== IFJ SYMBOLIC_TABLE BUILD ===\n"
	printf "=== make clean ===\n"
	make clean
	printf "=== make sem_test ===\n"
	make sem_test
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
				printf "\t\tNumber of stderr errors  = $number_of_errors_stderr\n"
				printf "\t\tExpected count of errors = $number_of_errors_file\n"
			fi
		fi

		if [ $ok = "=OK=" ]; then
			number_of_errors_stderr=$(cat stderr_file | grep ^.*ERR.*$ | wc -l)
			if [ $number_of_errors_stderr -ne 0 ]; then
				printf "\t\tNumber of stderr errors  = $number_of_errors_stderr\n"
				printf "\t\tExpected count of errors = 0\n"
			fi
		fi

	done

	rm stderr_file
fi

############################################################
if [ $switch_GOOD_dir_1 -eq 0 ]     || [ $switch_BAD_dir_1 -eq 0 ]   || [ $switch_BAD_dir_2 -eq 0 ]     ||
   [ $switch_MANUAL_CODES -eq 0 ]   || [ $switch_SCANNER_DIR -eq 0 ] || [ $switch_SCANNER_DIR_2 -eq 0 ] ||
   [ $switch_SYMBOLIC_TABLE -eq 0 ] || [ $switch_SCANNER_DIR_3 -eq 0]; then
	printf "\n\n=== MAKE CLEAN ===\n"
	make clean
	printf "\n\n=== FINISH ===\n"
else
	printf "=== TESTS ARE SWITCHED OFF ===\n=== FINISH ====\n"
fi