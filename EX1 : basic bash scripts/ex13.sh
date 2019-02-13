#!/bin/bash

file=$1
cd=$pwd

if [ $# -ne 1 ]
then
	echo ‫‪'error:‬‬ ‫‪only‬‬ ‫‪one‬‬ ‫‪argument‬‬ ‫‪is‬‬ allowed'
elif [ ! -f $file ]
then
	echo 'error:‬‬ ‫‪there‬‬ ‫‪is‬‬ ‫‪no‬‬ ‫‪such‬‬ file'
else 
	mkdir -p ‫‪safe_rm_dir‬‬ && cp $file $_
	rm $file
	echo '‫!‪done‬‬'
fi
