#!/bin/sh

ifj16_dir=pwd

############################################################
# ON  switch == 0 
# OFF swtich != 0

switch_GOOD_dir_1=0
GOOD_dir_1="`pwd`/tests/codes_without_error"

switch_BAD_dir_1=0
BAD_dir_1="`pwd`/tests/semantic_tests"

switch_BAD_dir_2=0
BAD_dir_2="`pwd`/tests/parser_tests"

switch_SCANNER_DIR=1
SCANNER_DIR="`pwd`/tests/scanner_tests"

############################################################
if [ $switch_GOOD_dir_1 -eq 0 ] || [ $switch_BAD_dir_1 -eq 0 ] || [ $switch_BAD_dir_2 -eq 0 ]; then
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
	printf "=== IF ("$""?" == 0) AND (ERROR == 0) THEN TEST PASSED ===\n"

	counter=0
	for i in `ls $GOOD_dir_1`
	do
		counter=$(expr $counter + 1)
		printf "= $counter.) $i \n"
	    ./ifj $GOOD_dir_1/$i >>subor 2>&1
	    exitvalue=$?
	    error=$(cat subor | grep ^ERR.* | wc -l)
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
	    error=$(cat subor | grep ^ERR.* | wc -l)
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
	    i_cond=$(echo $i | grep ^zdroj.*$ | wc -l) # ak je subor "zdroj......" tak i_cond=1 inak 0
	    if [ $i_cond -eq 1 ]; then
	        continue
	    fi
	    counter=$(expr $counter + 1)
	    printf "= $counter.) $i \n"
	    ./ifj $BAD_dir_2/$i >>subor 2>&1
	    exitvalue=$?
	    error=$(cat subor | grep ^ERR.* | wc -l)
	    if [ $exitvalue -eq 0 ] || [ $error -eq 0 ]; then
	        printf "\t"$""?"    = $exitvalue\n"
	        printf "\tERROR = %s\n" "$error"
	    fi
	    rm subor
	done
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
		i_cond=$(echo $i | grep ^.*output$ | wc -l)
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
printf "\n\n=== MAKE CLEAN ===\n"
make clean
printf "\n\n=== FINISH ===\n"