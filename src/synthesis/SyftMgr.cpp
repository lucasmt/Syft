#include "SyftMgr.h"

using std::make_shared;
using std::make_unique;
using std::move;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;

shared_ptr<BDDDict> SyftMgr::construct_vars(const Cudd& cudd_mgr,
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
    
SyftMgr::SyftMgr(const vector<DFA>& dfas, VarPartition partition)
  : var_partition(move(partition))
  , state_map(dfas, var_partition)
  , bdd_dict(construct_vars(cudd_mgr, var_partition, state_map))
{}

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
      size_t left = nodes[i].left_branch();
      ADD left_node = build_add_table(table, left, nodes);

      size_t right = nodes[i].right_branch();
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
