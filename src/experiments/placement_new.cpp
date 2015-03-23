/* 
 * File:   placement_new.cpp
 * Author: miche
 *
 * Created on 5 marzo 2015, 10.46
 */

#include <cstdlib>
#include "../graph-search.hpp"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    multi_task_t mymulti = new multi_task_t(3);
    
    mymulti.add_task("linea 1", 1);
    mymulti.add_task("linea 2 eh si", 2);
    mymulti.add_task("anche una linea 3", 3);
    
    for(int i = 0; i < mymulti.get_count(); i++){
        cout << mymulti.lines[i].linenum << " " << mymulti.lines[i].line << endl;
    }
        
    
    return 0;
}

