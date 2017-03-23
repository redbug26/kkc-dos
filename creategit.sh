#!/bin/bash

IFS=$'\n'

rm -rf .git
git init
rm -rf src
mkdir src

for file in $(find ../archives -maxdepth 1  -path "../archives/v*")
do
	echo $file  
	Regex='(..\/archives\/)([a-zA-Z0-9.()\/ ]*) - ([a-zA-Z0-9 ]*)'
	if [[ $file =~ $Regex ]]
	then
		date=`LC_ALL=c date -j -f "%d %b %Y" "${BASH_REMATCH[3]}" +"%a, %d %b %Y %T %z"`
		echo "  Date: ${BASH_REMATCH[3]}"
		echo "  Date: ${date}"
		echo "  Keyword: ${BASH_REMATCH[2]}"

		if [ "${BASH_REMATCH[2]}" == "v1.20 (new)" ] 
		then
			find src/*.c -type f -exec git mv {} {}c \;
			mkdir src/include
			find src/*.h -type f -exec git mv {} src/include \;
		fi

		cp -R "$file"/* src
		git add -A src
		GIT_COMMITTER_DATE="${date}" git commit --date="${date}" -m "${BASH_REMATCH[2]}"
	else
		echo "****** Your locale was not recognised"
	fi

done

git add -A
git commit -m "Ready for github"
