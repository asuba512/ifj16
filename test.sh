#!/bin/sh

ifj16_dir=pwd
GOOD_dir="`pwd`/tests/codes_without_error"
BAD_dir="`pwd`/tests/semantic_tests"

############################################################
printf "=== IFJ BUILD ===\n"
make clean
make

############################################################
printf "\n=== TESTS WITHOUT ERROR ===\n"

counter=0
for i in `ls $GOOD_dir`
do
	counter=$(expr $counter + 1)
	printf "== $counter.) $i \n"
    ./ifj $GOOD_dir/$i >file 2>&1 
    printf "\tECHO $ ? = $?\n"
    error=$(cat file | grep ^ERR.* | wc -l)
    printf "\tERROR    = %s\n" "$error"

    rm file
done

############################################################
printf "\n=== TESTS WITH ERROR ===\n"

counter=0
for i in `ls $BAD_dir`
do
	counter=$(expr $counter + 1)
	printf "== $counter.) $i \n"
    ./ifj $BAD_dir/$i >file 2>&1 
    printf "\tECHO $ ? = $?\n"
    error=$(cat file | grep ^ERR.* | wc -l)
    printf "\tERROR    = %s\n" "$error"

    rm file
done