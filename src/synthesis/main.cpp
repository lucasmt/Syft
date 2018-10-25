#include <iostream>
#include <string>
#include <memory>

#include "BDDMgr.hpp"

#include "optional.h"
#include "DFAGameSolver.h"
#include "DFALoader.h"
#include "SymbolicDFA.h"
#include "SymbolicDFAConverter.h"

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
    DFA dfa = DFALoader().run(filename, partfile);

    size_t number_of_bits = 0;

    for (size_t i = dfa.number_of_states - 1; i != 0; i >>= 1){
      ++number_of_bits;
    }

    jet::AttrRanking varRanking(dfa.number_of_vars + number_of_bits * 2);
    shared_ptr<BDDMgr> mgr = make_shared<BDDMgr>(varRanking);
    SymbolicDFA symbolic_dfa = SymbolicDFAConverter(mgr).run(dfa);
    
    DFAGameSolver solver(mgr);
    
    my::optional<unordered_map<unsigned, BDD>> strategy;
    
    //if(flag == "1")
    //    strategy = test.realizablity_variant();
    //else
        strategy = solver.realizablity(symbolic_dfa);

    if(strategy != my::nullopt)
        cout<<"realizable"<<endl;
    else
        cout<<"unrealizable"<<endl;
    return 0;

}
//solveeqn
