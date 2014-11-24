

DATA_DIR=../data
BUILD_DIR=build

test_sequential(){
    $BUILD_DIR/seq-graph-search $DATADIR/$1
}

usage(){
    echo -e "Usage: $0 <graph_file> <node>\n"
}

if [ $# -lt 2 ] ; then
    usage
    exit 0
fi

test_sequential $1 $2
