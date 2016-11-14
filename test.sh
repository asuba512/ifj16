#!/bin/sh

ifj16_dir=pwd
GOOD_dir="`pwd`/tests/codes_without_error"
BAD_dir="`pwd`/tests/semantic_tests"
BAD_dir_2="`pwd`/tests/parser_tests"

############################################################
printf "\n=== IFJ BUILD ===\n"
printf "=== make clean ===\n"
make clean
printf "=== make ===\n"
make

############################################################
dir=$(echo $GOOD_dir | sed "s/^.*\///")

printf "\n
=== TESTS WITHOUT ERROR ===
=== Directory: $dir ===
=== IF ("$""?" == 0) AND (ERROR == 0) THEN TEST PASSED ===\n"

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
dir=$(echo $BAD_dir | sed "s/^.*\///")

printf "\n
=== TESTS WITH ERROR ===
=== Directory: $dir ===
=== IF ("$""?" != 0) AND (ERROR != 0) THEN TEST PASSED ===\n"

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

############################################################
dir=$(echo $BAD_dir_2 | sed "s/^.*\///")

printf "\n
=== TESTS WITH ERROR ===
=== Directory: $dir ===
=== IF ("$""?" != 0) AND (ERROR != 0) THEN TEST PASSED ===
=== IGNORUJE TO ^zdroj.*$ subory ===\n"

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