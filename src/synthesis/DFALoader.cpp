#include "DFALoader.h"

#include "OldDFA.h"

using std::move;
using std::shared_ptr;
using std::string;

DFALoader::DFALoader(shared_ptr<Cudd> m)
  : mgr(move(m))
{}

DFA DFALoader::run(string filename, string partfile)
{
  OldDFA old(mgr);

  old.initialize(filename, partfile);

  // Primed state variables?
  for(int i = 0; i < old.nbits; i++){
    BDD b = mgr->bddVar();
    old.bddvars.push_back(b);
  }

  return DFA(move(old));
}
