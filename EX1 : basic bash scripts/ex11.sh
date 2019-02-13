#!/bin/bash

path=$1
count=0
#looping over files only, excludes directories, and updating a counter
for file in $path/*txt; do
    if [ ! -d "$file" ]; then
    ((count++))
    fi
done
echo '‫‪Number‬‬ ‫‪of‬‬ ‫‪files‬‬ ‫‪in‬‬ ‫‪the‬‬ ‫‪directory‬‬ ‫‪that‬‬ ‫‪end‬‬ ‫‪with‬‬ ‫‪.txt‬‬ ‫‪is‬‬' $count
