#include "DFA.h"

#include <memory>

using std::move;

DFA::DFA(OldDFA old)
{
  nbits = old.nbits;
  nvars = old.nvars;
  nstates = old.nstates;
  bddvars = move(old.bddvars);
  initbv = move(old.initbv);
  input = move(old.input);
  output = move(old.output);
  res = move(old.res);
  finalstatesBDD = move(finalstatesBDD);
}
