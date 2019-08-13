#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "cuddObj.hh"

#include "BucketElimination.hpp"
#include "ClusterAndJoin.hpp"
#include "GreedyScan.hpp"
#include "InOrderScan.hpp"
#include "TailJoin.hpp"

#include "optional.h"
#include "DFAGameSolver.h"
#include "DFALoader.h"
#include "JoinAlgFactory.h"
#include "Logger.h"
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

  if(argc != 7){
    cout << "Usage: " << argv[0]
         << " <base-filename>"
         << " <number-of-files>"
         << " <partition-file>"
         << " <join-algorithm>"
         << " <var-ranking>"
         << " <logging-option>"
         << endl;
      return 0;
    }

    string base_filename = argv[1];
    size_t k = atoi(argv[2]);
    string partition_file = argv[3];
    unordered_map<string, string> options;
    options["alg"] = argv[4];
    options["rank"] = argv[5];
    string logging_option = argv[6];

    unique_ptr<Logger> logger = Logger::build(logging_option);

    VarPartition var_partition = VarPartition::load(partition_file);

    vector<DFA> dfas;
    dfas.reserve(k);
    
    for (size_t i = 0; i < k; i++)
    {
      DFA dfa = DFA::load_from_file(base_filename, i, var_partition);
      dfas.push_back(dfa);
    }
    
    logger->record_dfas(dfas);

    SyftMgr mgr(dfas, move(var_partition));
    
    logger->record_vars(mgr);

    vector<SymbolicDFA> symbolic_dfas;
    symbolic_dfas.reserve(k);

    for (size_t i = 0; i < k; i++)
    {
      symbolic_dfas.push_back(SymbolicDFA(dfas[i], mgr));
    }

    logger->record_symbolic_dfas(symbolic_dfas, mgr);

    JoinAlgFactory factory(options);
    unique_ptr<jet::JoinAlgorithm> join_algorithm =
      factory.build(symbolic_dfas, mgr);
    FactoredSynthesizer synthesizer(mgr.bdd_dict, move(join_algorithm));
    DFAGameSolver solver(move(mgr), move(synthesizer));

    logger->start_timer();

    bool realizable = solver.realizability(symbolic_dfas, logger);

    logger->record_elapsed_time();

    logger->record_result(realizable);

    return 0;
}
