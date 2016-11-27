#!/bin/bash
 
generate=0
tmpdir="/tmp/ifj16/package"
xsubaa00=25
xpalie00=25
xsuhaj02=25
xtotha01=25

while getopts ":g" opt; do
  case $opt in
    g)
        generate=1
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done

rm -rf *.o ifj sc_test sem_test prec_test .fuse*
contents=`ls | grep -v -e "test" -e "docs" -e "pack" -e "README"`
mkdir -p $tmpdir
cp -r ${contents} $tmpdir

if [ $generate -eq 1 ]; then
    touch $tmpdir/dokumentace.pdf
fi
printf "xsubaa00:%.2d\nxtotha01:%.2d\nxsuhaj02:%.2d\nxpalie00:%.2d\n" $xsubaa00 $xtotha01 $xsuhaj02 $xpalie00 > $tmpdir/rozdeleni
printf "BOOLOP\n" > $tmpdir/rozsireni

currentdir=`pwd`
cd $tmpdir
tar -zcvf $currentdir/xsubaa00.tar.gz *
cd $currentdir