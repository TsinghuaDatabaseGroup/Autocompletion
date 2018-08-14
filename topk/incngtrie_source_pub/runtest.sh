#!/bin/bash

dataset_path="../data/"
dataset="author"
datak="10 20 30 40"
result=${2}
program=${1}

if [ $# -ne 2 ]; then
	echo Usage: ${0} program result_dir
	exit $E_BADARGS
fi

mkdir $result

#make clean && make

for name in $dataset; do
	for k in $datak; do
		for prefix in {7..8}; do
			echo $name k=$k prefix=$prefix
			cat ${dataset_path}${name}_query | cut -c1-${prefix} | sed -n -r /^.{${prefix}}/p | ./$program -f -k ${k} -c bro -d ${dataset_path}${name} 2>> ${result}/${name}.${k}
		done
	done
done
