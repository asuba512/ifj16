#!/bin/sh

ifj16_dir=pwd
GOOD_dir="`pwd`/tests/codes_without_error"
BAD_dir="`pwd`/tests/semantic_tests"

############################################################
printf "\n=== IFJ BUILD ===\n"
make clean
make

############################################################
printf "\n\n=== TESTS WITHOUT ERROR === IF ("$""?" == 0) AND (ERROR == 0) THEN TEST PASSED\n"

counter=0
for i in `ls $GOOD_dir`
do
	counter=$(expr $counter + 1)
	printf "= $counter.) $i \n"
    ./ifj $GOOD_dir/$i >>subor 2>&1
    exitvalue=$?
    error=$(cat subor | grep ^ERR.* | wc -l)
    if [ $exitvalue -ne 0 ] && [ $error -ne 0 ]; then
        printf "\t"$""?"    = $exitvalue\n"
        printf "\tERROR = %s\n" "$error"
    fi

    rm subor
done

############################################################
printf "\n\n=== TESTS WITH ERROR === IF ("$""?" != 0) AND (ERROR != 0) THEN TEST PASSED\n"

counter=0
for i in `ls $BAD_dir`
do
	counter=$(expr $counter + 1)
	printf "= $counter.) $i \n"
    ./ifj $BAD_dir/$i >>subor 2>&1
    exitvalue=$?
    error=$(cat subor | grep ^ERR.* | wc -l)
    if [ $exitvalue -eq 0 ] || [ $error -eq 0 ]; then
        printf "\t"$""?"    = $exitvalue\n"
        printf "\tERROR = %s\n" "$error"
    fi
    rm subor
done