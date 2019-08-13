#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "cuddObj.hh"

#include "optional.h"
#include "DFAGameSolver.h"
#include "DFALoader.h"
#include "JoinAlgFactory.h"
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
using std::unordered_map;
using std::vector;

int main(int argc, char ** argv){

  if(argc != 4){
    cout << "Usage: " << argv[0]
         << " <instance> <join-algorithm> <var-ranking>"
         << endl;
      return 0;
    }

  string instance_name = argv[1];
  unordered_map<string, string> options;
  options["alg"] = argv[2];
  options["rank"] = argv[3];

  InstanceFactory instance_factory;
  Instance instance = instance_factory.build(instance_name);

  /* OUTPUT TO CONSOLE */
  print_stats(instance);
  /* OUTPUT TO CONSOLE */

  JoinAlgFactory factory(options);
  unique_ptr<jet::JoinAlgorithm> join_algorithm =
    factory.build(symbolic_dfas, mgr);
  FactoredSynthesizer synthesizer(mgr.bdd_dict, move(join_algorithm));
  DFAGameSolver solver(move(mgr), move(synthesizer));

  bool realizable = solver.realizability(symbolic_dfas);

  /* OUTPUT TO CONSOLE */
  cout << "Result: " << (realizable ? "Realizable" : "Unrealizable")
       << endl;
  /* OUTPUT TO CONSOLE */

  return 0;
}
