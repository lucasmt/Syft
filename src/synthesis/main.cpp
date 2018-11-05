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

BDDDict construct_vars(Cudd& mgr,
		       const VarPartition& partition,
		       const StateMap& state_map)
{
  
}

int main(int argc, char ** argv){

  if(argc != 4){
      cout << "Usage: ./syft "
	   << "<base-filename> <number-of-files> <partition-file>"
	   << endl;
      return 0;
    }

    string base_filename = argv[1];
    size_t k = atoi(argv[2]);
    string partition_file = argv[3];

    VarPartition partition = VarPartition::load(partition_file);
    DFALoader loader(partition);

    Cudd mgr;
    BDDDict dict;

    vector<DFA> dfas;
    dfas.reserve(k);
    
    for (size_t i = 0; i < k; i++)
    {
      DFA dfa = loader.run(base_filename + i + ".dfa");
      dfas.push_back(dfa);
    }

    StateMap state_map(dfas, partition);
    BDDDict bdd_dict = construct_vars(mgr, partition, state_map);
    
    SymbolicDFAConverter symbolic_converter(state_map, bdd_dict);
    vector<SymbolicDFA> symbolic_dfas;
    symbolic_dfas.reserve(k);

    for (size_t i = 0; i < k; i++)
    {
      SymbolicDFA symbolic_dfa = symbolic_converter.run(dfa);
      symbolic_dfas.push_back(symbolic_dfa);
    }

    jet::AttrRanking bucket_ranking = compute_bucket_ranking(symbolic_dfas);
    unique_ptr<jet::JoinAlgorithm> join_algorithm =
      make_unique<jet::BucketElimination>(bucket_ranking);
    FactoredSynthesizer synthesizer(bdd_dict, join_algorithm);
    DFAGameSolver solver(state_map, bdd_dict, synthesizer);
    
    bool realizable = solver.realizability(symbolic_dfas);

    cout << (realizable ? "Realizable" : "Unrealizable") << endl;
    
    return 0;
}
