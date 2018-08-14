#!/bin/bash

dataset_path="../dataset/"
dataset="querylog"
result=${2}
program=${1}

if [ $# -ne 2 ]; then
	echo Usage: ${0} program result_dir
	exit $E_BADARGS
fi

mkdir $result

make clean && make

for name in $dataset; do
	for tau in {1..2}; do
		for prefix in {5..5}; do
			echo $name tau=$tau prefix=$prefix
			#		./$program $dataset_path$name ${dataset_path}${name}_query $tau > $result/$name.$tau
			cat ${dataset_path}${name}_query | cut -c -${prefix} | sed -n -r /^.{${prefix}}/p | ./$program -f -t ${tau} -c bro -d ${dataset_path}${name} 2>> ${result}/${name}.${tau} &
		done
	done
done
