#!/bin/bash
#
# Author: Michele Carignani <michele.carignani@gmail.com>
#

BUILD_DIR=./build
BINS="farm map farm-no-io"

PAR_DEGS="1 2 4 8 16 32 64"
GRANULARITIES="30 150 300 1500 3000 15000 30000"

par_test() {
    $BUILD_DIR/$1 $2 $3 $4 $5 2>&1
}

test_sequential(){ 
    $BUILD_DIR/seq $1 $2 2>&1
}

usage(){
    echo -e "Usage: $0 <graph_file> <nodes_file>\n"
}

if [ $# -lt 2 ] ; then
    usage
    exit 0
fi

echo "# Completion times in seconds of the different parallel and sequential versions"
echo "# using different computation grains and parallelism degree."
echo "# First line describes the format of the CSV records (semicolon separated)."
echo 
echo "Granularity; Program; 1; 2; 4; 8; 16; 32; 64"

seqRes=$( test_sequential $1 $2 )
seqRecordTail="seq "
for nw in $PAR_DEGS ; do
    seqRecordTail="$seqRecordTail; $seqRes"
done

for g in $GRANULARITIES ; do
    echo "$g; $seqRecordTail"
    for b in $BINS ; do
        record="$g; $b"
        for nw in $PAR_DEGS ; do
            res=$( par_test $b $1 $2 $nw $g )
            record="$record; $res"
        done
        echo $record
    done
done

echo -e "\n-- TESTS COMPLETED -- "
