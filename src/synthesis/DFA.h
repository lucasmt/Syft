#ifndef DFA_H
#define DFA_H

#include <memory>
#include <unordered_set>
#include <vector>

struct IOPartition
{
  std::unordered_set<std::string> input_vars;
  std::unordered_set<std::string> output_vars;
};

class DFA
{
public:

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
