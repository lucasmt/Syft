#ifndef DFA_H
#define DFA_H

#include <vector>

#include "Attr.hpp"

#include "SMTBDD.h"
#include "VarPartition.h"

class DFAState
{
  size_t _dfa_index;
  size_t _id;
  
 public:

  DFAState(size_t dfa_index, size_t id);
  
  size_t dfa_index() const;
  size_t id() const;
};

class DFA
{
 public:

  DFA(size_t index,
      size_t number_of_states,
      jet::AttrSet env_vars,
      jet::AttrSet sys_vars,
      size_t initial_state,
      SMTBDD transition_function,
      std::vector<size_t> accepting_states);
  
  size_t index() const;
  size_t number_of_states() const;
  
  jet::AttrSet env_vars() const;
  jet::AttrSet sys_vars() const;

  DFAState initial_state() const;
  
  const SMTBDD& transition_function() const;

  const std::vector<DFAState>& accepting_states() const;

  DFAState ith_state(size_t i) const;
  
  static DFA load_from_file(const std::string& base_filename,
                           size_t i,
                           const VarPartition& var_partition);
  
 private:

  size_t _index;
  size_t _number_of_states;
  jet::AttrSet _env_vars;
  jet::AttrSet _sys_vars;
  DFAState _initial_state;
  SMTBDD _transition_function;
  std::vector<DFAState> _accepting_states;
};

#endif // DFA_H
