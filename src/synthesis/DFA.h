#ifndef DFA_H
#define DFA_H

#include <memory>
#include <unordered_set>
#include <vector>

class DFAState
{
  size_t _dfa_index;
  size_t _id;
  
 public:

  DFAState(size_t dfa_index, size_t id);
  
  size_t dfa_index() const;
  size_t id() const;
}

class DFA
{
 public:

  DFA(size_t index,
      size_t number_of_states,
      VarPartition partition,
      size_t initial_state,
      SMTBDD transition_function,
      std::vector<size_t> accepting_states)
  
  size_t index() const;
  size_t number_of_states() const;
  
  jet::AttrSet env_vars() const;
  jet::AttrSet sys_vars() const;

  const SMTBDD& transition_function() const;

  const std::vector<DFAState>& accepting_states() const;
  
 private:

  size_t number_of_vars;
  std::vector<std::string> var_names;
  size_t number_of_states;
  size_t initial_state;
  size_t number_of_nodes;
  std::vector<size_t> final_states;
  std::vector<size_t> behaviour;
  std::vector<std::vector<size_t>> smtbdd;
  IOPartition partition;
};

#endif // DFA_H
