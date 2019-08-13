#include "Logger.h"

#include <iostream>
#include <numeric>
#include <stdexcept>
#include <vector>

using std::accumulate;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using std::cout;
using std::endl;
using std::make_unique;
using std::runtime_error;
using std::string;
using std::unique_ptr;
using std::vector;

unique_ptr<Logger> Logger::build(const string& logging_option)
{
  if (logging_option == "pretty")
  {
    return make_unique<PrettyLogger>();
  }
  else if (logging_option == "null")
  {
    return make_unique<NullLogger>();
  }
  else
  {
    throw runtime_error("Invalid logging option: " + logging_option);
  }
}

PrettyLogger::PrettyLogger()
{
  start_time = high_resolution_clock::now();
}

void PrettyLogger::record_message(const string& message)
{
  cout << message << endl;
}

void PrettyLogger::record_dfas(const vector<DFA>& dfas)
{
  cout << "DFA count: " << dfas.size() << endl;

  vector<size_t> state_counts;

  for (const DFA& dfa : dfas)
    state_counts.push_back(dfa.number_of_states());

  cout << "Avg DFA size: " << accumulate(state_counts.begin(),
                                         state_counts.end(),
                                         0.0) / (double) dfas.size() << endl;
}

void PrettyLogger::record_symbolic_dfas(const vector<SymbolicDFA>& dfas,
                                        const SyftMgr& mgr)
{
  vector<BDD> bdds;
  vector<size_t> node_counts;

  for (const SymbolicDFA& sdfa : dfas)
  {
    bdds.push_back(sdfa.transition_relation());
    bdds.push_back(sdfa.accepting_states());
    node_counts.push_back(sdfa.transition_relation().nodeCount());
  }

  cout << "Total node count: " << mgr.cudd_mgr.nodeCount(bdds) << endl;

  cout << "Avg (trans) node count: " << accumulate(node_counts.begin(),
                                                   node_counts.end(),
                                                   0.0) / (double) dfas.size()
       << endl;
}

void PrettyLogger::record_vars(const SyftMgr& mgr)
{
  cout << "Number of state vars: " << mgr.state_map.state_vars().size()
       << endl;

  cout << "Number of env vars: " << mgr.var_partition.env_vars().size()
       << endl;

  cout << "Number of sys vars: " << mgr.var_partition.sys_vars().size()
       << endl;
}

void PrettyLogger::record_winning_states(const BDD& winning_states)
{
  cout << "Winning states node count: "
       << winning_states.nodeCount() << endl;
}

void PrettyLogger::record_strategy(const vector<SkolemFunction>& strategy,
                                   const SyftMgr& mgr)
{
  cout << "System wins in: " << strategy.size() << " moves" << endl;

  vector<BDD> bdds;

  for (const SkolemFunction& sf : strategy)
  {
    for (jet::Attr var : sf.outputVars())
    {
      bdds.push_back(sf[var]);
    }
  }

  cout << "Strategy node count: " << mgr.cudd_mgr.nodeCount(bdds) << endl;
}

void PrettyLogger::record_result(bool realizable)
{
  cout << "Result: " << (realizable ? "Realizable" : "Unrealizable")
       << endl;
}

void PrettyLogger::start_timer()
{
  start_time = high_resolution_clock::now();
}

void PrettyLogger::record_elapsed_time()
{
  auto end_time = high_resolution_clock::now();

  cout << "Elapsed time: "
       << duration_cast<milliseconds>(end_time - start_time).count()
       << endl;
}
