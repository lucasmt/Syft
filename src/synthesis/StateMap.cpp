#include "StateMap.h"

#include <cassert>
#include <stdexcept>

using std::invalid_argument;
using std::vector;

StateMap::StateMap(const vector<DFA>& dfas,
		   const VarPartition& partition)
{
  size_t last_id = partition.max_id();
  size_t lower_bound = 0;

  for (size_t i = 0; i < dfas.size(); i++) {

    assert (dfas[i].number_of_states() > 0);
    
    size_t max_state = dfas[i].number_of_states() - 1;

    // iterate for the number of shifts until it becomes 0
    while (max_state != 0) {
      _state_vars.push_back(++last_id);
      _next_state_vars.push_back(++last_id);
      
      max_state = max_state >> 1;
    }

    size_t upper_bound = _state_vars.size();
    _dfa_bounds.emplace_back(lower_bound, upper_bound);
    lower_bound = upper_bound;
  }
}


jet::Attr StateMap::prime(jet::Attr var) const
{
  auto position = lower_bound(_state_vars.begin(), _state_vars.end(), var);

  if (position == _state_vars.end() || *position != var)
    throw invalid_argument("Tried to prime non-state variable: " + var.id());
  
  size_t index = position - _state_vars.begin();
  
  return _next_state_vars[index];
}  

jet::AttrSet StateMap::state_vars() const
{
  return _state_vars;
}

jet::AttrSet StateMap::next_state_vars() const
{
  return _next_state_vars;
}

jet::AttrSet StateMap::state_vars(size_t dfa_index) const
{
  return jet::AttrSet(_state_vars.begin() + _dfa_bounds[dfa_index].first,
  		      _state_vars.begin() + _dfa_bounds[dfa_index].second);
}

jet::AttrSet StateMap::next_state_vars(size_t dfa_index) const
{
  return jet::AttrSet(_next_state_vars.begin() + _dfa_bounds[dfa_index].first,
  		      _next_state_vars.begin() + _dfa_bounds[dfa_index].second);
}

jet::AttrSet StateMap::state_vars(const DFA& dfa) const
{
  return state_vars(dfa.index());
}

jet::AttrSet StateMap::next_state_vars(const DFA& dfa) const
{
  return next_state_vars(dfa.index());
}

jet::Attr StateMap::state_var(const DFA& dfa, size_t var_index) const
{
  size_t i = dfa.index();
  size_t lower_bound = _dfa_bounds[i].first;

  // check if var_index is within the bounds?
  
  return _state_vars[lower_bound + var_index];
}

jet::Attr StateMap::next_state_var(const DFA& dfa, size_t var_index) const
{
  size_t i = dfa.index();
  size_t lower_bound = _dfa_bounds[i].first;

  // check if var_index is within the bounds?
  
  return _next_state_vars[lower_bound + var_index];
}

size_t StateMap::vars_per_dfa(const DFA& dfa) const
{
  size_t i = dfa.index();
  
  return _dfa_bounds[i].second - _dfa_bounds[i].first;
}

Assignment StateMap::encode(const DFAState& state,
                            const vector<jet::Attr>& vars) const
{
  size_t i = state.dfa_index();
  size_t id = state.id();
  vector<jet::Attr> assigned_to_true;

  auto begin = vars.begin() + _dfa_bounds[i].first;
  auto end = vars.begin() + _dfa_bounds[i].second;
  
  for (auto it = begin; it != end && id != 0; ++it) {
    size_t bit = id & 1; // take the least-significant bit

    if (bit == 1) assigned_to_true.push_back(*it);

    id = id >> 1;
  }

  return Assignment(jet::AttrSet(assigned_to_true));
}

Assignment StateMap::encode_current(const DFAState& state) const
{
  return encode(state, _state_vars);
}

Assignment StateMap::encode_next(const DFAState& state) const
{
  return encode(state, _next_state_vars);
}
