#ifndef DFA_H
#define DFA_H

#include "OldDFA.h"

class DFA
{
public:
  
  DFA(OldDFA old);
  
  int nbits;
  int nvars;
  int nstates;
  std::vector<BDD> bddvars;

  std::vector<int> initbv;
  std::vector<int> input;
  std::vector<int> output;
  
  std::vector<BDD> res;

  BDD finalstatesBDD;
};

#endif // DFA_H
