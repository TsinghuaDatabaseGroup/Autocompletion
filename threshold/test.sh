#!/bin/bash

programs="findMatchBinary compact compact+early compact+dominate compact+early+dominate"
branches="traverse binary hybrid compact+hybrid compact+binary"
externs="ipcan ican"
scales="word author querylog"

finding=false
fetching=false
comparing=false
scaling=false

if [ $# -lt 1 ]; then
	echo Usage: ${0} "[all|finding|fetching|comparing|scaling]"
	exit $E_BADARGS
fi

for arg in $@; do
	if [ $arg = "finding" ]; then
		finding=true
	elif [ $arg = "fetching" ]; then
		fetching=true
	elif [ $arg = "comparing" ]; then
		comparing=true
	elif [ $arg = "scaling" ]; then
		scaling=true
	elif [ $arg = "all" ]; then
		finding=true
		fetching=true
		comparing=true
		scaling=true
	fi
done

echo finding=$finding fetching=$fetching comparing=$comparing scaling=$scaling

if $finding; then
	echo ------------------------------
	echo "        finding"
	for prog in ${programs}; do
		echo Testing ${prog}
		cd $prog
		cp ../runtest.sh .
		./runtest.sh sigmod15 result/
		cd ..
	done
fi

if $fetching; then
	echo -------------------------------
	echo "        fetching"
	cd compact+early+dominate
	cp ../runtest.sh .
	for branch in $branches; do
		echo Testing $branch
		git checkout $branch
		./runtest.sh sigmod15 ${branch}_result/
	done
	cd ..
fi

if $comparing; then
	echo -------------------------------
	echo "        comparing"
	for dir in $externs; do
		echo Testing $dir
		cd $dir
		./runtest.sh ipcan result/
		cd ..
	done
fi

if $scaling; then
	echo -------------------------------
	echo "        scaling"
	cd compact+early+dominate
	for dir in $scales; do
		echo Testing $dir
		git checkout compact+binary
		make clean && make
		if [ -e result_${dir}_scale ]; then
			rm result_${dir}_scale
		fi
		mkdir result_${dir}_scale
		for file in ../dataset/${dir}_scale/*; do
			for tau in {1..4}; do
				echo ----testing $file, tau=${tau}
				./sigmod15 $file ../dataset/${dir}_query ${tau} > result_${dir}_scale/`basename ${file}`.${tau}
			done
		done
	done
	cd ..
fi
