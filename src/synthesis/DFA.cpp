#include "DFA.h"

#include <memory>

using std::move;
using std::vector;

DFA::DFA(size_t nb,
         DFAArgs args,
         DFABDDs bdds,
         vector<int> initbv,
         IOPartition part)
{
  nbits = nb;
  nvars = args.nvars;
  nstates = args.nstates;
  bddvars = move(bdds.bddvars);
  initbv = move(initbv);
  input = move(part.input);
  output = move(part.output);
  res = move(bdds.res);
  finalstatesBDD = move(bdds.finalstatesBDD);
}
