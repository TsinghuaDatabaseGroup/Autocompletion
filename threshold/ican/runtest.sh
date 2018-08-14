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

#make clean && make

for name in $dataset; do
	for tau in {1..4}; do
		echo $name $tau
		./$program $dataset_path$name ${dataset_path}${name}_query $tau > $result/$name.$tau &
	done
done
