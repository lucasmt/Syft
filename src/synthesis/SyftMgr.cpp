#include "SyftMgr.h"

using std::make_shared;
using std::make_unique;
using std::move;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;

shared_ptr<BDDDict> SyftMgr::construct_vars(const vector<DFA>& dfas,
					    const Cudd& cudd_mgr,
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
    jet::AttrRanking::MCS(vertices, hyperedges);//.reverse();
  
  return make_shared<BDDDict>(cudd_mgr, ordering);
}

shared_ptr<BDDDict> SyftMgr::construct_vars_0(const Cudd& cudd_mgr,
					      const VarPartition& var_partition,
					      const StateMap& state_map) const
{
  vector<jet::Attr> ordering;

  jet::AttrSet state_vars = state_map.state_vars();
  jet::AttrSet next_state_vars = state_map.next_state_vars();

  /*
  auto it1 = state_vars.begin(), it2 = next_state_vars.begin();

  while (it1 != state_vars.end()) {
    ordering.push_back(*it1);
    ordering.push_back(*it2);
    ++it1;
    ++it2;
  }
  */

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

/* Fully interleave state variables */
shared_ptr<BDDDict> SyftMgr::construct_vars_1(const Cudd& cudd_mgr,
					      const VarPartition& var_partition,
					      const StateMap& state_map) const
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

  return make_shared<BDDDict>(cudd_mgr, jet::AttrRanking(ordering));
}

/* Interleave only the DFAs */
shared_ptr<BDDDict> SyftMgr::construct_vars_2(const vector<DFA>& dfas,
                                              const Cudd& cudd_mgr,
					      const VarPartition& var_partition,
					      const StateMap& state_map) const
{
  vector<jet::Attr> ordering;

  for (const DFA& dfa : dfas)
  {
    jet::AttrSet state_vars = state_map.state_vars(dfa);
    jet::AttrSet next_state_vars = state_map.next_state_vars(dfa);

    ordering.insert(ordering.end(), state_vars.begin(), state_vars.end());
    ordering.insert(ordering.end(),
                    next_state_vars.begin(),
                    next_state_vars.end());
  }

  jet::AttrSet env_vars = var_partition.env_vars();
  jet::AttrSet sys_vars = var_partition.sys_vars();

  ordering.insert(ordering.end(), env_vars.begin(), env_vars.end());
  ordering.insert(ordering.end(), sys_vars.begin(), sys_vars.end());

  return make_shared<BDDDict>(cudd_mgr, jet::AttrRanking(ordering));
}
    
SyftMgr::SyftMgr(const vector<DFA>& dfas, VarPartition partition)
  : var_partition(move(partition))
  , state_map(dfas, var_partition)
  , bdd_dict(construct_vars_0(cudd_mgr, var_partition, state_map))
{
  cudd_mgr.AutodynEnable();
}

ADD SyftMgr::add_of_var(jet::Attr var) const
{
  unsigned int index = bdd_dict->bddOfVar(var).NodeReadIndex();

  return cudd_mgr.addVar(index);
}

ADD SyftMgr::build_add_table(vector<unique_ptr<ADD>>& table,
                             size_t i,
                             const vector<SMTBDDNode>& nodes) const
{
  if (!table[i])
  {
    if (nodes[i].is_terminal())
    {
      size_t value = nodes[i].terminal_value();

      table[i] = make_unique<ADD>(cudd_mgr.constant(value));
    }
    else
    {
      size_t left = nodes[i].left_index();
      ADD left_node = build_add_table(table, left, nodes);

      size_t right = nodes[i].right_index();
      ADD right_node = build_add_table(table, right, nodes);

      jet::Attr var = nodes[i].var();
      table[i] = make_unique<ADD>(add_of_var(var).Ite(right_node, left_node));
    }
  }

  return *table[i];
}

vector<ADD> SyftMgr::interpret(const SMTBDD& smtbdd) const
{
  const vector<SMTBDDNode>& nodes = smtbdd.nodes();
  const vector<size_t>& behavior = smtbdd.behavior();
  
  vector<unique_ptr<ADD>> table(nodes.size());
  vector<ADD> transition_function;
  transition_function.reserve(behavior.size());
  
  for (size_t index : behavior)
  {
    ADD add = build_add_table(table, index, nodes);
    //report("Bit 0: ", add.BddIthBit(0));
    //report("Bit 1: ", add.BddIthBit(1));
    transition_function.push_back(add);
  }

  return transition_function;
}

jet::AttrSet SyftMgr::output_vars() const
{
  jet::AttrSet sys_vars = var_partition.sys_vars();
  jet::AttrSet next_state_vars = state_map.next_state_vars();
  
  return sys_vars.unionWith(next_state_vars);
}

BDD SyftMgr::minterm(const DFAState& state) const
{
  Assignment symbolic_state = state_map.encode_current(state);

  return minterm(state.dfa_index(), symbolic_state);
}

BDD SyftMgr::minterm(size_t dfa_index, const Assignment& symbolic_state) const
{
  jet::AttrSet setToTrue = symbolic_state.assignedToTrue();
  jet::AttrSet allStateVars = state_map.state_vars(dfa_index);
  jet::AttrSet setToFalse = allStateVars.differenceWith(setToTrue);

  BDD minterm = cudd_mgr.bddOne();

  for (jet::Attr var : setToTrue) minterm &= bdd_dict->bddOfVar(var);
  for (jet::Attr var : setToFalse) minterm &= !bdd_dict->bddOfVar(var);

  return minterm;
}
