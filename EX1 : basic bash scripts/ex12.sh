#!/bin/bash

path=$1
#looping over files only, cheking if it's a directory or file
for item in $path/*; do
	if [ -d "$item" ]; then
		echo $(basename "$item") '‫‪is‬‬ ‫‪a‬‬ ‫‪directory‬‬'
	else
    		echo $(basename "$item") '‫‪is‬‬ ‫‪a‬‬ ‫‪file‬‬'
	fi
done

