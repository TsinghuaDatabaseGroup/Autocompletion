#!/bin/bash

dataset_path="../dataset/"
dataset=("word" "querylog" "author")
datacount=(4 5 6)
datatau=(3 4 5)
result=${2}
program=${1}

if [ $# -ne 2 ]; then
	echo Usage: ${0} program result_dir
	exit $E_BADARGS
fi

if [ -e ./$result ]; then
	rm -rf ./$result
fi
mkdir $result

make clean && make

for (( i=0; i<3; ++i)); do
    name=${dataset[$i]}
    count=${datacount[$i]}
    tau=${datatau[$i]}
    for (( L=1; L<=$count; ++L)); do
		echo $name $L
		./$program $dataset_path${name}_scale/${name}_${L} ${dataset_path}${name}_query $tau | tee $result/$name.$L
	done
done
