#!/bin/bash

dataset_path="../data/"
dataset=("word" "querylog" "author")
datacount=(4 5 6)
datak=(10 20 40 30)
result=${2}
program=${1}

if [ $# -ne 2 ]; then
	echo Usage: ${0} program result_dir
	exit $E_BADARGS
fi

mkdir $result

#make clean && make

for (( i=0; i<3; i++ )); do
    for (( j=0; j<4; j++ )); do
        K=${datak[$j]}
        dir=${dataset[$i]}
        for file in ../data/${dir}_scale/*; do
            echo ----testing $file, K=${K}
	        ./$program $file ${dataset_path}${dir}_query $K | tee $result/`basename ${file}`.$K
        done
    done
done
