#include <vector>

#include "Attr.hpp"

#include "Assignment.hpp"

#include "DFA.h"
#include "VarPartition.h"

class StateMap
{
 public:

  StateMap(const std::vector<DFA>& dfas, const VarPartition& partition);

  jet::Attr prime(jet::Attr state_var) const;
  
  jet::AttrSet state_vars() const;
  jet::AttrSet next_state_vars() const;

  jet::AttrSet state_vars(const DFA& dfa) const;
  jet::AttrSet next_state_vars(const DFA& dfa) const;

  Assignment encode_current(const DFAState& state) const;
  Assignment encode_next(const DFAState& state) const;
  
 private:

  std::vector<jet::Attr> _state_vars;
  std::vector<jet::Attr> _next_state_vars;

  std::vector<std::pair<size_t, size_t>> _dfa_bounds;

  Assignment encode(const DFAState& state,
		    const std::vector<jet::Attr>& var_sets) const;
};
