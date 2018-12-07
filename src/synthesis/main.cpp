#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "cuddObj.hh"

#include "BucketElimination.hpp"

#include "optional.h"
#include "DFAGameSolver.h"
#include "DFALoader.h"
#include "SyftMgr.h"
#include "SymbolicDFA.h"
#include "SymbolicDFAConverter.h"

using std::string;
using std::unique_ptr;
using std::make_unique;
using std::move;
using std::cout;
using std::endl;
using std::unique_ptr;
using std::vector;

/*
BDDDict construct_vars(Cudd& cudd_mgr,
		       const VarPartition& var_partition,
		       const StateMap& state_map)
{
  vector<jet::Attr> ordering;
  
  jet::AttrSet state_vars = state_map.state_vars();
  jet::AttrSet next_state_vars = state_map.next_state_vars();
  
  auto it1 = state_vars.begin(), it2 = next_state_vars.begin();
  
  while (it1 != state_vars.end()) {
    ordering.push_back(*it1);
    ordering.push_back(*it2);
    ++it1;
    ++it2;
  }
  
  jet::AttrSet env_vars = var_partition.env_vars();
  jet::AttrSet sys_vars = var_partition.sys_vars();

  ordering.insert(ordering.end(), env_vars.begin(), env_vars.end());
  ordering.insert(ordering.end(), sys_vars.begin(), sys_vars.end());

  return BDDDict(cudd_mgr, jet::AttrRanking(ordering));
}
*/

jet::AttrRanking compute_bucket_ranking(const vector<SymbolicDFA>& dfas,
                                        const SyftMgr& mgr)
{
  jet::AttrSet vertices = mgr.output_vars();

  vector<jet::AttrSet> hyperedges;
  hyperedges.reserve(dfas.size());
  
  for (const SymbolicDFA& dfa : dfas)
  {
    hyperedges.push_back(dfa.output_vars());
  }

  return jet::AttrRanking::MCS(vertices, hyperedges);
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

    VarPartition var_partition = VarPartition::load(partition_file);

    vector<DFA> dfas;
    dfas.reserve(k);
    
    for (size_t i = 0; i < k; i++)
    {
      DFA dfa = DFA::load_from_file(base_filename, i, var_partition);
      dfas.push_back(dfa);
    }
    
    SyftMgr mgr(dfas, move(var_partition));
    
    vector<SymbolicDFA> symbolic_dfas;
    symbolic_dfas.reserve(k);

    for (size_t i = 0; i < k; i++)
    {
      symbolic_dfas.push_back(SymbolicDFA(dfas[i], mgr));
    }

    jet::AttrRanking bucket_ranking =
      compute_bucket_ranking(symbolic_dfas, mgr);
    
    unique_ptr<jet::JoinAlgorithm> join_algorithm =
      make_unique<jet::BucketElimination>(bucket_ranking);

    FactoredSynthesizer synthesizer(mgr.bdd_dict, move(join_algorithm));
    DFAGameSolver solver(move(mgr), move(synthesizer));

    bool realizable = solver.realizability(symbolic_dfas);

    return 0;
}
