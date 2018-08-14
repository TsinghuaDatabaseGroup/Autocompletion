#!/bin/bash

dataset="word querylog author"

for dir in $dataset; do
	if [ $dir = "word" ]; then
		prefix=3
	fi
	if [ $dir = "querylog" ]; then
		prefix=5
	fi
	if [ $dir = "author" ]; then
		prefix=4
	fi
	if [ -e $dir ]; then rm $dir; fi
	touch $dir
	for tau in {1..4}; do
		echo -n "$tau " >> $dir
		cat ../tau_result/$dir.$tau | sed -n "${prefix},${prefix}p" >> $dir
	done
done
