#!/bin/sh
# 
# File:   simple-scalability-test.sh
# Author: miche
#
# Created on 2-feb-2015, 16.05.44
#
# Tests completion times of different program versions
# on one dataset and one needles list, using different number of workers
#

BUILD_DIR=./build
BINS="farm map farm-no-io"

PAR_DEGS="1 2 4 8 16 32 64 128 238"

DEFAULT_GRANULARITY=100

DATASETS="100 10000 1M 7M"
NEEDLES="2 100 250"

par_test() {
    $BUILD_DIR/$1 $2 $3 $4 $5 2>&1
}

test_sequential(){ 
    $BUILD_DIR/seq $1 $2 2>&1
}

test_sequential(){ 
    $BUILD_DIR/seq $1 $2 2>&1
}

usage(){
    echo -e "Usage: $0 <graph_file_basename> <nodes_file_basename>\n"
}

if [ $# -lt 2 ] ; then
    usage
    exit 0
fi

echo "# Completion times in seconds of the different parallel and sequential versions"
echo "# using parallelism degree."
echo "# First line describes the format of the CSV records (semicolon separated)."
echo 
echo "# Arguments: $1 $2"
echo

if [ ! -f "$1" ] ; then
	echo -e "Cannot find dataset $1"
	exit -1
fi

if [ ! -f "$2" ] ; then
	echo -e "Cannot find needles $2"
	exit -1
fi

$BUILD_DIR/profile none $2
echo
echo "Program; 1; 2; 4; 8; 16; 32; 64; 128; 238"

# Test the sequential version and print it many times
seqRes=$( test_sequential "$1" "$2" )
seqRecordTail="seq "
for nw in $PAR_DEGS ; do
    seqRecordTail="$seqRecordTail; $seqRes"
done
echo $seqRecordTail

for b in $BINS ; do
	record="$b"
	for nw in $PAR_DEGS ; do
            res=$( par_test $b "$1" "$2" $nw $DEFAULT_GRANULARITY )
		record="$record; $res"
    done
	echo $record
done