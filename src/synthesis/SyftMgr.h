
#ifndef SYFTMGR_H
#define SYFTMGR_H

#include <memory>
#include <vector>

#include "cuddObj.hh"

#include "Attr.hpp"

#include "BDDDict.hpp"

#include "SMTBDD.h"
#include "StateMap.h"
#include "VarPartition.h"

class SyftMgr
{
public:

  SyftMgr(const std::vector<DFA>& dfas, VarPartition partition);

  const VarPartition var_partition;
  const StateMap state_map;
  const Cudd cudd_mgr;
  const std::shared_ptr<BDDDict> bdd_dict;

  std::vector<ADD> interpret(const SMTBDD& smtbdd) const;
  
  jet::AttrSet output_vars() const;

  BDD minterm(const DFAState& state) const;
  BDD minterm(size_t dfa_index, const Assignment& symbolic_state) const;

private:

  std::shared_ptr<BDDDict> construct_vars(const std::vector<DFA>& dfas,
					  const Cudd& cudd_mgr,
					  const VarPartition& var_partition,
					  const StateMap& state_map) const;
  
  std::shared_ptr<BDDDict> construct_vars_0(const Cudd& cudd_mgr,
					    const VarPartition& var_partition,
					    const StateMap& state_map) const;

  ADD add_of_var(jet::Attr var) const;
  
  ADD build_add_table(std::vector<std::unique_ptr<ADD>>& table,
                      size_t i,
                      const std::vector<SMTBDDNode>& nodes) const;
};

#endif // SYFTMGR_H
