#!/bin/bash
 
generate=0
tests=0
tmpdir="/tmp/ifj16/package"
xsubaa00=25
xpalie00=25
xsuhaj02=25
xtotha01=25
declare -A contributors
declare -A contributorsName
declare -A tmpCon
while getopts ":gt" opt; do
  case $opt in
    g)
        generate=1
      ;;
    t)
        tests=1
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done

contributorsName[xsubaa00]="Adam Šuba"
contributorsName[xpalie00]="Jakub Paliesek"
contributorsName[xsuhaj02]="Peter Šuhaj"
contributorsName[xtotha01]="Adrián Tóth"

make clean
rm -rf dokumentace.pdf xsubaa00.tgz

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

	for login in "${!contributors[@]}"
	do
		authors=$authors`echo "${contributorsName[$login]} ($login)\0"`
	done

  authors=${authors:0:-2}
  message="/********************************************************************\n *\n * Projekt: Implementace interpretu imperativniho jazyka IFJ16\n * Řešitelé: ${authors//"\0"/, }\n *\n ********************************************************************/\n\n"

  sed -i '1s@^@'"$message"'@' $file

  cd $currentdir
  contributors=()
  tmpCon=()
  authors=""
done

if [ $generate -eq 1 ]; then
    touch $tmpdir/dokumentace.pdf
else
	make doc
	cp dokumentace.pdf $tmpdir
	rm dokumentace.pdf
fi
printf "xsubaa00:%.2d\nxtotha01:%.2d\nxsuhaj02:%.2d\nxpalie00:%.2d\n" $xsubaa00 $xtotha01 $xsuhaj02 $xpalie00 > $tmpdir/rozdeleni
printf "BOOLOP\n" > $tmpdir/rozsireni

if [ $tests -eq 1 ]; then
  contents=`ls | grep -e "test" | grep -v -e "[.]c" -e "[.]h"`
  cp -r ${contents} $tmpdir
fi

currentdir=`pwd`
cd $tmpdir
tar -zcvf $currentdir/xsubaa00.tgz *
cd $currentdir
rm -rf $tmpdir
