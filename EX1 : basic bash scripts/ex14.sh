#!/bin/bash
name=$1
file=$2
balance=0
while read -r line
do
        arr=($line)
	# check if line starts with the given name (through my code I also treated cases which no last name exists or name is longer
	#otherwise code would be much simpler, but this is my choice for a more rubost code)
	if [[ $line == $name* ]] ; then
	line_without_name=${line#$name};
	arr2=($line_without_name)
	#add number to balance	
	balance=$((balance + ${arr2[@]:0:1}))	
	echo $line
	fi
done <"$file"
echo $balance
