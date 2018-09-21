#include <iostream>
#include <string>
#include <memory>

#include "optional.h"
#include "syn.h"

using std::string;
using std::shared_ptr;
using std::make_shared;
using std::move;
using std::cout;
using std::endl;
using std::unordered_map;

int main(int argc, char ** argv){
    string filename;
    string partfile;
    string flag;
    if(argc != 4){
        cout<<"Usage: ./Syft DFAfile Partfile Starting_player(0: system, 1: environment)"<<endl;
        return 0;
    }
    else{
        filename = argv[1];
        partfile = argv[2];
        flag = argv[3];
    }
    shared_ptr<Cudd> mgr = make_shared<Cudd>();
    syn test(move(mgr), filename, partfile);
    
    my::optional<unordered_map<unsigned, BDD>> strategy;
    
    //if(flag == "1")
    //    strategy = test.realizablity_variant();
    //else
        strategy = test.realizablity();

    if(strategy != my::nullopt)
        cout<<"realizable"<<endl;
    else
        cout<<"unrealizable"<<endl;
    return 0;

}
//solveeqn
