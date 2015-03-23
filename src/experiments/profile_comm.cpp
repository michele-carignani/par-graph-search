/* 
 * File:   profile_comm.cpp
 * Author: miche
 *
 * Created on 10 marzo 2015, 15.40
 */

#include <cstdlib>
#include <ff/node.hpp>

using namespace std;
using namespace ff;


class Emitter : public ff_node {
public:
    void* svc(void* t){
        clock_gettime(CLOCK_REALTIME, &start);
        ff_send_out( (long) 1 );
        clock_gettime(CLOK_REALTIME, &end)
    }

};

class Receiver : public ff_node {
public:
    void* svc(){
        
    }
};

/*
 * 
 */
int main(int argc, char** argv) {
    
    return 0;
}

