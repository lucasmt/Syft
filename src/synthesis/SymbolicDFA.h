#ifndef SYMBOLICDFA_H
#define SYMBOLICDFA_H

#include "cuddObj.hh"

class SymbolicDFA
{
  jet::AttrSet env_vars;
  jet::AttrSet sys_vars;
  jet::AttrSet state_vars;
  jet::AttrSet next_state_vars;

  jet::AttrSet input_vars;
  jet::AttrSet output_vars;

  Assignment initial_assignment;

  BDD transition_relation;

  BDD accepting_states;
  
public:

  jet::AttrSet env_vars() const;
  jet::AttrSet sys_vars() const;
  jet::AttrSet input_vars() const;
  jet::AttrSet output_vars() const;

  Assignment initial_assignment() const;
  BDD transition_relation() const;
  BDD accepting_states() const;
  /*  
  size_t number_of_bits;
  size_t number_of_vars;
  size_t number_of_states;

  std::vector<int> initial_bitvector;
  std::vector<size_t> input_indices;
  std::vector<size_t> output_indices;

  std::vector<BDD> bdd_vars;
  
  std::vector<BDD> transition_function;

  BDD final_states;
  */
};

#endif // SYMBOLICDFA_H
