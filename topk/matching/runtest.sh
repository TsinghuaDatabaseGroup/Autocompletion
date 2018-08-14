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

if [ ! -e ./$result ]; then
    mkdir $result
fi

make clean && make

for name in $dataset; do
	for tau in $datak; do
		echo $name $tau
		./$program $dataset_path$name ${dataset_path}${name}_query $tau > $result/$name.$tau
	done
done
