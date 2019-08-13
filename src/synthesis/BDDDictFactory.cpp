#include "BDDDictFactory.h"

#include <stdexcept>

using std::move;
using std::runtime_error;
using std::shared_ptr;
using std::string;
using std::vector;

BDDDictFactory::BDDDictFactory(string option)
  : _option(move(option))
{}

jet::AttrRanking BDDDictFactory::build_mcs(const vector<DFA>& dfas,
                                           const VarPartition& var_partition,
                                           const StateMap& state_map) const
{
  jet::AttrSet vertices =
    var_partition.env_vars()
    .unionWith(var_partition.sys_vars())
    .unionWith(state_map.state_vars())
    .unionWith(state_map.next_state_vars());

  vector<jet::AttrSet> hyperedges;
  hyperedges.reserve(dfas.size());

  for (const DFA& dfa : dfas)
  {
    jet::AttrSet all_vars =
      dfa.env_vars()
      .unionWith(dfa.sys_vars())
      .unionWith(state_map.state_vars(dfa.index()))
      .unionWith(state_map.next_state_vars(dfa.index()));

    hyperedges.push_back(all_vars);
  }

  jet::AttrRanking ordering =
    jet::AttrRanking::MCS(vertices, hyperedges);

  return ordering;
}

shared_ptr<BDDDict> BDDDictFactory::build(const vector<DFA>& dfas,
                                          const Cudd& cudd_mgr,
                                          const VarPartition& var_partition,
                                          const StateMap& state_map) const
{
  if (_option == "original")
  {
    vector<jet::Attr> ordering;

    jet::AttrSet state_vars = state_map.state_vars();
    jet::AttrSet next_state_vars = state_map.next_state_vars();
    jet::AttrSet env_vars = var_partition.env_vars();
    jet::AttrSet sys_vars = var_partition.sys_vars();

    ordering.insert(ordering.end(), state_vars.begin(), state_vars.end());
    ordering.insert(ordering.end(), env_vars.begin(), env_vars.end());
    ordering.insert(ordering.end(), sys_vars.begin(), sys_vars.end());
    ordering.insert(ordering.end(),
                    next_state_vars.begin(),
                    next_state_vars.end());

    return make_shared<BDDDict>(cudd_mgr, jet::AttrRanking(ordering));
  }
  else if (_option == "mcs")
  {
    jet::AttrRanking ordering = build_mcs(dfas, var_partition, state_map);

    return make_shared<BDDDict>(cudd_mgr, ordering);
  }
  else if (_option == "reverse-mcs")
  {
    jet::AttrRanking ordering = build_mcs(dfas, var_partition, state_map);

    return make_shared<BDDDict>(cudd_mgr, ordering.reverse());
  }
  else
  {
    throw runtime_error("Invalid variable ordering: " + _option);
  }
}
