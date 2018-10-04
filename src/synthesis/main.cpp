#include <iostream>
#include <string>
#include <memory>

#include "optional.h"
#include "DFAGameSolver.h"
#include "DFALoader.h"

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
    DFALoader loader(mgr);
    DFA dfa = loader.run(filename, partfile);
    
    DFAGameSolver test(mgr);
    
    my::optional<unordered_map<unsigned, BDD>> strategy;
    
    //if(flag == "1")
    //    strategy = test.realizablity_variant();
    //else
        strategy = test.realizablity(dfa);

    if(strategy != my::nullopt)
        cout<<"realizable"<<endl;
    else
        cout<<"unrealizable"<<endl;
    return 0;

}
//solveeqn
