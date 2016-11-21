#!/bin/sh
ifj16_dir=pwd

############################################################
# ON  switch == 0 
# OFF swtich != 0

switch_GOOD_dir_1=0
GOOD_dir_1="`pwd`/tests/codes_without_error"

switch_BAD_dir_1=1
BAD_dir_1="`pwd`/tests/semantic_tests"

switch_BAD_dir_2=1
BAD_dir_2="`pwd`/tests/parser_tests"

# SCANNER TESTS - the new folder
switch_SCANNER_DIR_2=1
SCANNER_DIR_2="`pwd`/tests/scanner_tests_2"

# SCANNER TESTS are for nothing because the behaviour was changed with FQID
switch_SCANNER_DIR=1 # keep it OFF!
SCANNER_DIR="`pwd`/tests/scanner_tests"

switch_SYMBOLIC_TABLE=1
SYMBOLIC_TABLE="`pwd`/tests/symbol_table_tests"

# expecting that the code is without error and the existatus of "./ifj FILE" is 0
switch_MANUAL_CODES=1
MANUAL_CODES="`pwd`/tests/codes_from_manual"

############################################################
printf "\n\n=== IFJ BUILD ===\n"
printf "=== make clean ===\n"
make clean
printf "=== make ===\n"
make

############################################################
if [ $switch_GOOD_dir_1 -eq 0 ]; then

	dir=$(echo $GOOD_dir_1 | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS WITHOUT ERROR ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF ("$""?" == 0) AND (ERROR == 0) THEN TEST PASSED ===\n"

	counter=0
	for i in `ls $GOOD_dir_1`
	do
		counter=$(expr $counter + 1)
		printf "= $counter.) $i \n"
	    ./ifj $GOOD_dir_1/$i >>subor 2>&1
	    exitvalue=$?
	    error=$(cat subor | grep ^.*ERR.*$ | wc -l)
	    if [ $exitvalue -ne 0 ] && [ $error -ne 0 ]; then
	        printf "\t"$""?"    = $exitvalue\n"
	        printf "\tERROR = %s\n" "$error"
	    fi
	    rm subor
	done
fi

############################################################
if [ $switch_BAD_dir_1 -eq 0 ]; then
	
	dir=$(echo $BAD_dir_1 | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS WITH ERROR ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF ("$""?" != 0) AND (ERROR != 0) THEN TEST PASSED ===\n"

	counter=0
	for i in `ls $BAD_dir_1`
	do
		counter=$(expr $counter + 1)
		printf "= $counter.) $i \n"
	    ./ifj $BAD_dir_1/$i >>subor 2>&1
	    exitvalue=$?
	    error=$(cat subor | grep ^.*ERR.*$ | wc -l)
	    if [ $exitvalue -eq 0 ] || [ $error -eq 0 ]; then
	        printf "\t"$""?"    = $exitvalue\n"
	        printf "\tERROR = %s\n" "$error"
	    fi
	    rm subor
	done
fi

############################################################
if [ $switch_BAD_dir_2 -eq 0 ]; then
	
	dir=$(echo $BAD_dir_2 | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS WITH ERROR ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF ("$""?" != 0) AND (ERROR != 0) THEN TEST PASSED ===\n"
	printf "=== IGNORUJE TO ^zdroj.*$ subory ===\n"

	counter=0
	for i in `ls $BAD_dir_2`
	do
	    i_cond=$(echo $i | grep ^zdroj.*$ | wc -l) # file name starts with "zdroj..." then skip this file
	    if [ $i_cond -eq 1 ]; then
	        continue
	    fi

	    counter=$(expr $counter + 1)
	    printf "= $counter.) $i \n"
	    ./ifj $BAD_dir_2/$i >>subor 2>&1
	    exitvalue=$?
	    error=$(cat subor | grep ^.*ERR.*$ | wc -l)
	    if [ $exitvalue -eq 0 ] || [ $error -eq 0 ]; then
	        printf "\t"$""?"    = $exitvalue\n"
	        printf "\tERROR = %s\n" "$error"
	    fi
	    rm subor
	done
fi

############################################################
if [ $switch_MANUAL_CODES -eq 0 ]; then
	
	dir=$(echo $MANUAL_CODES | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS MANUAL CODES ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF ("$""?" == 0) AND (STDERR is EMPTY) THEN TEST PASSED ===\n"

	counter=0
	> empty_file
	for i in `ls $MANUAL_CODES`
	do
	    counter=$(expr $counter + 1)
	    printf "= $counter.) $i \n"
	    ./ifj $MANUAL_CODES/$i 2>stderr_subor
	    exitvalue=$?
	    diff stderr_subor empty_file > /dev/null
	    exitvalue_diff=$?
	    if [ $exitvalue -ne 0 ] || [ $exitvalue_diff -ne 0 ]; then
	        printf "\t"$""?" = $exitvalue\n"
	        printf "\tSTDERR include errors\n"
	    fi
	    rm stderr_subor
	done
	rm empty_file
fi

############################################################
if [ $switch_SCANNER_DIR -eq 0 ]; then
	
	dir=$(echo $SCANNER_DIR | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS DIFF ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF stdout == .output THEN TEST PASSED ===\n"
	printf "=== make clean ===\n"
	make clean
	printf "=== make test ===\n"
	make test
	printf "\n"

	counter=0
	for i in `ls $SCANNER_DIR`
	do
		i_cond=$(echo $i | grep ^.*output$ | wc -l) # file name ends with "...output" then skip this file
		if [ $i_cond -eq 1 ]; then
			continue
		fi

	    counter=$(expr $counter + 1)
	    printf "= $counter.) $i \n"
	    ./sc_test "$SCANNER_DIR/$i" >subor
	 	diff subor "$SCANNER_DIR/$i.output" > /dev/null
	    exitvalue=$?
	    if [ $exitvalue -ne 0 ]; then
	        printf "\tDIFF "$""?" = 1 (different output)\n"
	    fi
	    rm subor
	done
fi

############################################################
if [ $switch_SCANNER_DIR_2 -eq 0 ]; then
	
	dir=$(echo $SCANNER_DIR_2 | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS DIFF ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF stdout == .output THEN TEST PASSED ===\n"
	printf "=== make clean ===\n"
	make clean
	printf "=== make test ===\n"
	make test
	printf "\n"

	counter=0
	for i in `ls $SCANNER_DIR_2`
	do
		i_cond=$(echo $i | grep ^.*output$ | wc -l) # file name ends with "...output" then skip this file
		if [ $i_cond -eq 1 ]; then
			continue
		fi

	    counter=$(expr $counter + 1)
	    printf "= $counter.) $i \n"
	    ./sc_test "$SCANNER_DIR_2/$i" >subor
	 	diff subor "$SCANNER_DIR_2/$i.output" > /dev/null
	    exitvalue=$?
	    if [ $exitvalue -ne 0 ]; then
	        printf "\tDIFF "$""?" = 1 (different output)\n"
	    fi
	    rm subor
	done
fi

############################################################
if [ $switch_SYMBOLIC_TABLE -eq 0 ]; then
	
	dir=$(echo $SYMBOLIC_TABLE | sed "s/^.*\///")
	printf "\n\n"
	printf "=== TESTS SYMBOLIC TABLE ===\n"
	printf "=== Directory: $dir ===\n"
	printf "=== IF stdout == .output THEN TEST PASSED ===\n"
	printf "=== make clean ===\n"
	make clean
	printf "=== make sem_test ===\n"
	make sem_test
	printf "=== make done ===\n"

	counter=0
	for i in `ls $SYMBOLIC_TABLE`
	do
		i_cond=$(echo $i | grep ^.*output$ | wc -l) # file name ends with "...output" then skip this file
		if [ $i_cond -eq 1 ]; then
			continue
		fi
	    
	    counter=$(expr $counter + 1)
	    printf "= $counter.) $i \n"
	    ./sem_test "$SYMBOLIC_TABLE/$i" >subor 2>stderr_subor

	    ok=$(cat $SYMBOLIC_TABLE/$i.output | head -1)
	    if [ $ok = "=ERROR=" ]; then
	   		number_of_errors_file=$(cat $SYMBOLIC_TABLE/$i.output | head -2 | tail -1 | awk '{print $3}')
	   		number_of_errors_stderr=$(cat stderr_subor | grep ^.*ERR.*$ | wc -l)
	   		if [ $number_of_errors_file -ne $number_of_errors_stderr ]; then
				printf "\tNumber of stderr errors  = $number_of_errors_stderr\n"
				printf "\tExpected count of errors = $number_of_errors_file\n"	   		
	   		fi
	    fi
	    if [ $ok = "=OK=" ]; then
	    	number_of_errors_stderr=$(cat stderr_subor | grep ^.*ERR.*$ | wc -l)
	    	if [ $number_of_errors_stderr -ne 0 ]; then
				printf "\tNumber of stderr errors  = $number_of_errors_stderr\n"
				printf "\tExpected count of errors = 0\n"		    		
	    	fi
	    fi
	    rm stderr_subor
	    rm subor
	done
fi

############################################################
printf "\n\n=== MAKE CLEAN ===\n"
make clean
printf "\n\n=== FINISH ===\n"