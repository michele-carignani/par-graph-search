#!/bin/sh
# 
# File:   correctness.sh
# Author: michele.carignani@gmail.com
#
#

if [ $# -lt 3 ] ; then
	echo "Usage: $0 <graph_file> <needles_file> <results_file>"
	exit
fi

GRAPH_FILE=$1
NEEDLES_FILE=$2
RESULTS_FILE=$3

BIN_DIR=./build
TMP_DIR=/tmp
BINS="seq seq-no-io farm farm-no-io map"

sorted_res=$TMP_DIR/$(basename $RESULTS_FILE).sorted
sort $RESULTS_FILE > $sorted_res

for b in $BINS ; do
	tmpfile=$TMP_DIR/res_$b_$(date "+%s")
	./$BIN_DIR/$b $GRAPH_FILE $NEEDLES_FILE > $tmpfile 2>/dev/null
	sort $tmpfile > $tmpfile.sorted
	diff $sorted_res $tmpfile.sorted > /dev/null 
	if [ $? -ne 0 ] ; then
		echo -e "$b : fail"
		echo "-- EXPECTED : --"
		cat $sorted_res
		echo "-- RECEIVED : --"
		cat $tmpfile.sorted
		echo -e "-- --\n"
	else
		echo -e "$b : success\n"
	fi
	rm -f $tmpfile $tmpfile.sorted
done

rm -f $sorted_res
