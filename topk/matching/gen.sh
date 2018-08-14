#!/bin/bash

dataset="word querylog author"

for dir in $dataset; do
	if [ $dir = "word" ]; then
		prefix=6
	fi
	if [ $dir = "querylog" ]; then
		prefix=10
	fi
	if [ $dir = "author" ]; then
		prefix=8
	fi
    datak="10 20 30 40"
	for tau in $datak; do
		if [ -e $dir.$tau ]; then
			rm $dir.$tau
		fi
		touch $dir.$tau
		count=0
		for file in `ls ../scale_result/${dir}_*.${tau} | sort -V`; do
			count=$((count+1))
			echo -n "$count " >> $dir.$tau
			cat $file | sed -n "${prefix},${prefix}p" >> $dir.$tau 
		done
	done
done
