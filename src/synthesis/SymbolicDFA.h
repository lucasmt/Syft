#ifndef SYMBOLICDFA_H
#define SYMBOLICDFA_H

#include "cuddObj.hh"

#include "Attr.hpp"

#include "Assignment.hpp"

#include "DFA.h"
#include "SyftMgr.h"

class SymbolicDFA
{
  jet::AttrSet _env_vars;
  jet::AttrSet _sys_vars;
  jet::AttrSet _state_vars;
  jet::AttrSet _next_state_vars;

  jet::AttrSet _input_vars;
  jet::AttrSet _output_vars;

  Assignment _initial_assignment;

  BDD _transition_relation;

  BDD _accepting_states;

  BDD construct_bdd_new(const DFA& dfa, const SyftMgr& mgr) const;

  std::vector<BDD> try_get(size_t index,
                           std::vector<std::vector<BDD>>& tBDD,
                           const std::vector<std::vector<size_t>>& smtbdd,
                           size_t nbits,
                           const std::vector<BDD>& bdd_vars) const;
  
public:

  SymbolicDFA(const DFA& dfa, const SyftMgr& mgr);
  
  jet::AttrSet env_vars() const;
  jet::AttrSet sys_vars() const;
  jet::AttrSet input_vars() const;
  jet::AttrSet output_vars() const;

  Assignment initial_assignment() const;
  BDD transition_relation() const;
  BDD accepting_states() const;
};

#endif // SYMBOLICDFA_H
