#include "JoinAlgFactory.h"

#include <stdexcept>

#include "BucketElimination.hpp"
#include "Clustering.hpp"
#include "GreedyScan.hpp"
#include "InOrderScan.hpp"
#include "TailJoin.hpp"

using std::make_unique;
using std::move;
using std::runtime_error;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

JoinAlgFactory::JoinAlgFactory(
  unordered_map<string, string> options)
  : _options(move(options))
{}

jet::AttrRanking JoinAlgFactory::compute_mcs(const vector<SymbolicDFA>& dfas,
                                             const jet::AttrSet& vertices,
                                             const SyftMgr& mgr)
{
  vector<jet::AttrSet> hyperedges;
  hyperedges.reserve(dfas.size());

  for (const SymbolicDFA& dfa : dfas)
  {
    hyperedges.push_back(dfa.output_vars().intersectWith(vertices));
  }

  return jet::AttrRanking::MCS(vertices, hyperedges);
}

jet::AttrRanking JoinAlgFactory::compute_ranking(
  const vector<SymbolicDFA>& dfas,
  const jet::AttrSet& relevant_vars,
  const SyftMgr& mgr)
{
  string rank = _options.at("rank");

  if (rank == "mcs")
  {
    return compute_mcs(dfas, relevant_vars, mgr);
  }
  else if (rank == "reverse-mcs")
  {
    return compute_mcs(dfas, relevant_vars, mgr).reverse();
  }
  else if (rank == "counters-1")
  {
    vector<jet::Attr> vars;

    vars.push_back(mgr.var_partition.from_name("INC_SYS"));

    size_t n = (mgr.var_partition.sys_vars().size() - 1) / 4;

    for (size_t i = 0; i < n; ++i)
    {
      vars.push_back(mgr.var_partition.from_name("CARRY_SYS_" + to_string(i)));
      vars.push_back(mgr.var_partition.from_name("CARRY_ENV_" + to_string(i)));
    }

    for (size_t i = 0; i < n; ++i)
    {
      vars.push_back(mgr.var_partition.from_name("COUNTER_SYS_" + to_string(i)));
      vars.push_back(mgr.var_partition.from_name("COUNTER_ENV_" + to_string(i)));
    }

    return jet::AttrRanking(vars);
  }
  else if (rank == "counters-2")
  {
    vector<jet::Attr> vars;

    vars.push_back(mgr.var_partition.from_name("INC_SYS"));

    size_t n = (mgr.var_partition.sys_vars().size() - 1) / 4;

    for (size_t i = 0; i < n; ++i)
    {
      vars.push_back(mgr.var_partition.from_name("CARRY_SYS_" + to_string(i)));
      vars.push_back(mgr.var_partition.from_name("CARRY_ENV_" + to_string(i)));
      vars.push_back(mgr.var_partition.from_name("COUNTER_SYS_" + to_string(i)));
      vars.push_back(mgr.var_partition.from_name("COUNTER_ENV_" + to_string(i)));
    }

    return jet::AttrRanking(vars);
  }
  else
  {
    throw runtime_error("Invalid ranking: " + rank);
  }
}

unique_ptr<jet::JoinAlgorithm> JoinAlgFactory::build(
  const vector<SymbolicDFA>& dfas,
  const SyftMgr& mgr)
{
  string alg = _options.at("alg");

  if (alg == "bucket-elimination")
  {
    jet::AttrRanking ranking = compute_ranking(dfas, mgr.output_vars(), mgr);

    return make_unique<jet::BucketElimination>(ranking);
  }
  else if (alg == "greedy-scan")
  {
    return make_unique<jet::GreedyScan>(mgr.output_vars());
  }
  else if (alg == "in-order")
  {
    return make_unique<jet::InOrderScan>(mgr.output_vars());
  }
  else if (alg == "tail-join")
  {
    jet::AttrRanking ranking =
      compute_ranking(dfas, mgr.var_partition.sys_vars(), mgr);

    unique_ptr<jet::JoinAlgorithm> join1 =
      make_unique<jet::BucketElimination>(ranking);
    unique_ptr<jet::JoinAlgorithm> join2 =
      make_unique<jet::InOrderScan>(mgr.state_map.next_state_vars());

    return make_unique<jet::TailJoin>(move(join1), move(join2));
  }
  else
  {
    throw runtime_error("Invalid join algorithm: " + alg);
  }
}
