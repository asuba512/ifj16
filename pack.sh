#!/bin/bash
 
generate=0
tmpdir="/tmp/ifj16/package"
xsubaa00=25
xpalie00=25
xsuhaj02=25
xtotha01=25

declare -A contributors
declare -A tmpCon

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

for file in $contents
do
  if [ $file = "Makefile" ]; then
    continue
  fi
  tmp=`git shortlog -s -n -- $file | tr -d " " | tr "\t" ":" | awk 'BEGIN{FS=":";OFS="="}{ t=$1; $1="["$2"]"; $2=t;print; }' | tr "\n" " "`
  eval "tmpCon=($tmp)"
  for contr in "${!tmpCon[@]}"
  do
      if [ $contr = "AdamSuba" ] && [ ${tmpCon[$contr]} -ge 4 ]
      then
        contributors[xsubaa00]=${tmpCon[$contr]}
      fi
      if ([ $contr = "xpalie00" ] || [ $contr = "xpalie42" ]) && [ ${tmpCon[$contr]} -ge 2 ]
      then
        contributors[xpalie00]=`expr ${contributors[xpalie00]} + ${tmpCon[$contr]}`
      fi
      if [ $contr = "peter2141" ] && [ ${tmpCon[$contr]} -ge 2 ]
      then
        contributors[xsuhaj02]=${tmpCon[$contr]}
      fi
      if ([ $contr = "AdriánTóth" ] || [ $contr = "europ" ]) && [ ${tmpCon[$contr]} -ge 2 ]
      then
        contributors[xtotha01]=`expr ${contributors[xtotha01]} + ${tmpCon[$contr]}`
      fi
  done

  currentdir=`pwd`
  cd $tmpdir

  authors=`echo ${!contributors[@]} | tr " " "\n" | sort | tr "\n" " "`
  authors=${authors:0:-1}
  message="/***\n * IFJ16 programming language interpret\n * Contributors: ${authors// /, }\n***/\n\n"

  sed -i '1s@^@'"$message"'@' $file

  cd $currentdir
  contributors=()
  tmpCon=()
done

if [ $generate -eq 1 ]; then
    touch $tmpdir/dokumentace.pdf
fi
printf "xsubaa00:%.2d\nxtotha01:%.2d\nxsuhaj02:%.2d\nxpalie00:%.2d\n" $xsubaa00 $xtotha01 $xsuhaj02 $xpalie00 > $tmpdir/rozdeleni
printf "BOOLOP\n" > $tmpdir/rozsireni

currentdir=`pwd`
cd $tmpdir
tar -zcvf $currentdir/xsubaa00.tgz *
cd $currentdir
rm -rf $tmpdir