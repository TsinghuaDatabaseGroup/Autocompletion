#!/bin/bash

dataset="word querylog author"

for dir in $dataset; do
	if [ $dir = "word" ]; then
		prefix=3
	fi
	if [ $dir = "querylog" ]; then
		prefix=7
	fi
	if [ $dir = "author" ]; then
		prefix=5
	fi
	for tau in {1..4}; do
		if [ -e $dir.$tau ]; then
			rm $dir.$tau
		fi
		touch $dir.$tau
		count=0
		for file in `ls ../result_${dir}_scale/${dir}_*.${tau} | sort -V`; do
			count=$((count+1))
			echo -n "$count " >> $dir.$tau
			cat $file | sed -n "${prefix},${prefix}p" >> $dir.$tau 
		done
	done
done
