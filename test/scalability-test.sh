	#!/bin/bash
#
# Author: Michele Carignani <michele.carignani@gmail.com>
#

BUILD_DIR=./build
BINS="farm map-reduce"

GRANULARITIES="300 3000 30000 300000"

par_test() {
    $BUILD_DIR/$1 $2 $3 $4 $5 2>&1
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

date
echo
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

if [ $3 = 'mic' ] ; then
	PAR_DEGS="1 2 4 8 16 32 64 128 238"
	header="Program; Grain; 1; 2; 4; 8; 16; 32; 64; 128; 238"
else
	PAR_DEGS="1 2 4 8 14"
	header="Program; Grain; 1; 2; 4; 8; 14"
fi


echo $header


$BUILD_DIR/profile none $2

seqRes=$( test_sequential "$1" "$2" )
seqRecordTail="seq "
for nw in $PAR_DEGS ; do
    seqRecordTail="$seqRecordTail; $seqRes"
done

for g in $GRANULARITIES ; do
    echo "$g; $seqRecordTail"
    for b in $BINS ; do
        record="$g; $b"
        for nw in $PAR_DEGS ; do
            res=$( par_test $b "$1" "$2" $nw $g )
            record="$record; $res"
        done
        echo $record
    done
done

echo -e "\n# TESTS COMPLETED"
