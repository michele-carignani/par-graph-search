#!/bin/bash/
#
# Author: Michele Carignani <michele.carignani@gmail.com>

BUILD_DIR=./build

PAR_DEGS="1 2 4 8 16 32 64 128"
WORKERS_NUM=16

par_test() {
	$BUILD_DIR/$1 $2 $3 $4 $5 1>/dev/null
}

test_sequential(){
    $BUILD_DIR/seq $1 $2 1>/dev/null
}

usage(){
    echo -e "Usage: $0 <graph_file> <nodes_file>\n"
}

if [ $# -lt 2 ] ; then
    usage
    exit 0
fi

echo "\n-- DIFFERENT GRAIN TESTS -- "

echo -e "\n# Test FARM"
for i in $PAR_DEGS ; do
	par_test "farm" $1 $2 16 $i
	echo -e "\t"
done

echo -e "\n# Test PIPE-FARM"
for i in $PAR_DEGS ; do
	par_test "pipe-farm" $1 $2 16 $i
	echo -e "\t"
done

echo -e "\n# Test MAP"
for i in $PAR_DEGS ; do
	par_test "map" $1 $2 16 $i
	echo -e "\t"
done

echo -e "\n-- TESTS COMPLETED -- "