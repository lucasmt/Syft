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

jet::AttrRanking compute_var_ranking(const DFA& dfa)
{
    size_t number_of_bits = 0;

    for (size_t i = dfa.number_of_states - 1; i != 0; i >>= 1){
      ++number_of_bits;
    }

    return jet::AttrRanking(dfa.number_of_vars + number_of_bits * 2);  
}

jet::AttrSet vars_to_project(const SymbolicDFA& dfa)
{
  vector<jet::Attr> vars;

  for
}

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

    shared_ptr<BDDMgr> mgr = make_shared<BDDMgr>(compute_var_ranking(dfa));
    SymbolicDFA symbolic_dfa = SymbolicDFAConverter(mgr).run(dfa);

    std::unique_ptr<jet::JoinAlgorithm> joinAlgorithm =
      make_unique<MonolithicJoin>(symbolic_dfa.output_vars());
    FactoredSynthesizer synthesizer(mgr, joinAlgorithm);
    DFAGameSolver solver(mgr, synthesizer);
    
    //my::optional<unordered_map<unsigned, BDD>> strategy;
    
    //if(flag == "1")
    //    strategy = test.realizablity_variant();
    //else
    //    strategy = solver.realizablity(symbolic_dfa);

    bool realizable = solver.realizability(symbolic_dfa);
    
    if(realizable)
        cout<<"realizable"<<endl;
    else
        cout<<"unrealizable"<<endl;
    return 0;

}
//solveeqn
