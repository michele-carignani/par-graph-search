#!/bin/bash
# 
# File:   prepare-test.sh.bash
# Author: miche
#
# Created on 14-gen-2015, 0.15.14
#

MIC_PREFIX_DIR=$1
DATASET=$2
NEEDLES_FILE=$3

DATASETS="100 10000 1M 7M"
NEEDLES="2 100 250"

echo -e "\nCopying executables on the mic.."
ssh mic0 "mkdir -p $MIC_PREFIX_DIR/build"
for i in $(ls build/mic) ; do 
    scp build/mic/$i mic0:$MIC_PREFIX_DIR/build/$i ; 
done

echo -e "\nCopying test files on the mic.."

ssh mic0 "mkdir -p $MIC_PREFIX_DIR/test"
scp test/scalability-test.sh mic0:$MIC_PREFIX_DIR/test/scalability-test.sh

ssh mic0 "mkdir -p $MIC_PREFIX_DIR/data"
scp data/$DATASET mic0:$MIC_PREFIX_DIR/data/$DATASET
scp data/$NEEDLES_FILE mic0:$MIC_PREFIX_DIR/data/$NEEDLES_FILE

for d in $DATASETS; do
	ssh mic0 "head -n $d $MIC_PREFIX_DIR/data/$DATASET > $MIC_PREFIX_DIR/data/$DATASET.$d"
done

for n in $NEEDLES; do
	ssh mic0 "head -n $n $MIC_PREFIX_DIR/data/$NEEDLES_FILE > $MIC_PREFIX_DIR/data/$NEEDLES_FILE.$n"
done