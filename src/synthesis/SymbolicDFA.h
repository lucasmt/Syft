#ifndef SYMBOLICDFA_H
#define SYMBOLICDFA_H

#include "cuddObj.hh"

class SymbolicDFA
{
  jet::AttrSet env_vars;
  jet::AttrSet sys_vars;
  jet::AttrSet state_vars;
  jet::AttrSet next_state_vars;

  Assignment initial_assignment;

  BDD transition_relation;

  BDD accepting_states;
  
public:
  
  size_t number_of_bits;
  size_t number_of_vars;
  size_t number_of_states;

  std::vector<int> initial_bitvector;
  std::vector<size_t> input_indices;
  std::vector<size_t> output_indices;

  std::vector<BDD> bdd_vars;
  
  std::vector<BDD> transition_function;

  BDD final_states;
};

#endif // SYMBOLICDFA_H
