#ifndef DFA_H
#define DFA_H

#include "cuddObj.hh"

struct DFAArgs
{
  size_t nvars;
  std::vector<std::string> variables;
  size_t nstates;
  size_t init;
  size_t nodes;
  std::vector<size_t> finalstates;
  std::vector<size_t> behaviour;
  std::vector<std::vector<size_t>> smtbdd;
};

struct DFABDDs
{
  std::vector<BDD> bddvars;
  std::vector<BDD> res;
  BDD finalstatesBDD;
};

struct IOPartition
{
  std::vector<size_t> input;
  std::vector<size_t> output;
};

class DFA
{
public:
  
  DFA(size_t nb,
      DFAArgs args,
      DFABDDs bdds,
      std::vector<int> initbv,
      IOPartition part);
  
  size_t nbits;
  size_t nvars;
  size_t nstates;
  std::vector<BDD> bddvars;

  std::vector<int> initbv;
  std::vector<size_t> input;
  std::vector<size_t> output;
  
  std::vector<BDD> res;

  BDD finalstatesBDD;
};

#endif // DFA_H
