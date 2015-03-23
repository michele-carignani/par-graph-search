	#!/bin/bash
#
# Author: Michele Carignani <michele.carignani@gmail.com>
#

BUILD_DIR=./build
BINS="farm map farm-no-io"

PAR_DEGS="1 2 4 8 16 32 64 128 238"
GRANULARITIES="300 3000 30000"

DATASETS="100 10000 1M 7M"
NEEDLES="2 100 250"

par_test() {
    $BUILD_DIR/$1 $2 $3 $4 $5 2>&1
}

test_sequential(){ 
    $BUILD_DIR/seq $1 $2 2>&1
}

test_sequential_no_io(){ 
    $BUILD_DIR/seq-no-io $1 $2 2>&1
}

usage(){
    echo -e "Usage: $0 <graph_file_basename> <nodes_file_basename>\n"
}

if [ $# -lt 2 ] ; then
    usage
    exit 0
fi

echo "# Completion times in seconds of the different parallel and sequential versions"
echo "# using different computation grains and parallelism degree."
echo "# First line describes the format of the CSV records (semicolon separated)."
echo 
echo "# Arguments: $1 $2"
echo

for f in $DATASETS ; do
	if [ ! -f "$1.$f" ] ; then
		echo -e "Cannot find dataset $1.$f"
		exit -1
	fi
done

for n in $NEEDLES ; do
	if [ ! -f "$2.$n" ] ; then
		echo -e "Cannot find needles $2.$n"
		exit -1
	fi
done

echo "Edges; Needles; Granularity; Program; 1; 2; 4; 8; 16; 32; 64; "

for d in $DATASETS; do
for n in $NEEDLES; do
$BUILD_DIR/profile none $2.$n
seqRes=$( test_sequential "$1.$d" "$2.$n" )
seqRecordTail="seq "
for nw in $PAR_DEGS ; do
    seqRecordTail="$seqRecordTail; $seqRes"
done
seqnoioRes=$( test_sequential_no_io "$1.$d" "$2.$n" )
seqnoioRecordTail="seq "
for nw in $PAR_DEGS ; do
    seqnoioRecordTail="$seqRecordTail; $seqRes"
done
for g in $GRANULARITIES ; do
    echo "$d;$n;$g; $seqRecordTail"
    for b in $BINS ; do
        record="$d;$n;$g; $b"
        for nw in $PAR_DEGS ; do
            res=$( par_test $b "$1.$d" "$2.$n" $nw $g )
            record="$record; $res"
        done
        echo $record
    done
done
done
done

echo -e "\n# TESTS COMPLETED"
