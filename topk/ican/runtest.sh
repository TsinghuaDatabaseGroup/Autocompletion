#!/bin/bash

dataset_path="../data/"
dataset="word querylog author"
datak="10 20 30 40 50"
result=${2}
program=${1}

if [ $# -ne 2 ]; then
	echo Usage: ${0} program result_dir
	exit $E_BADARGS
fi

mkdir $result

make clean && make

for name in $dataset; do
	for K in $datak; do
		echo $name $K
		./$program $dataset_path$name ${dataset_path}${name}_query $K > $result/$name.$K &
	done
done
