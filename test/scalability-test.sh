

DATA_DIR=data
BUILD_DIR=./build

PAR_DEGS="2 4 8 16 32 64"

TIME_FORMAT_STRING="%e\t%U\t%S"

par_test() {
	$BUILD_DIR/$1 $DATA_DIR/$2 $3 $4 $5 1>/dev/null
}

test_sequential(){
    $BUILD_DIR/seq $DATA_DIR/$1 $2 1>/dev/null
}

usage(){
    echo -e "Usage: $0 <graph_file> <nodes_file>\n"
}

if [ $# -lt 2 ] ; then
    usage
    exit 0
fi

echo -e "\n# Test SEQ"
test_sequential $1 $2
echo -e "\t"

echo -e "\n# Test FARM"
for i in $PAR_DEGS ; do
	par_test "farm" $1 $2 $i
	echo -e "\t"
done

echo -e "\n# Test PIPE-FARM"
for i in $PAR_DEGS ; do
	par_test "pipe-farm" $1 $2
	echo -e "\t"
done

echo -e "\n# Test MAP"
for i in $PAR_DEGS ; do
	par_test "map" $1 $2
	echo -e "\t"
done

echo "\n-- TESTS COMPLETED -- "