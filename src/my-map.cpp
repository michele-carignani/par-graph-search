#include <ff/parallel_for.hpp>
#include <iostream>

using namespace ff;
using namespace std;
int main(int argc, char* argv[]){
    assert(argc>1);
    long N = atol(argv[1]);
    long *A = new long[N];

    ParallelFor pf;

    pf.parallel_for(0,N,[&A](const long i) {A[i] = i;});
    pf.parallel_for(0, N, 2, [&A](const long i) {A[i]=i*i;});
    for(long i = 0; i < N; i++) std::cout << A[i] << " ";
    std::cout << "\n";
    return 0;
}