#ifndef DFAGAMESOLVER_H
#define DFAGAMESOLVER_H

#include <memory>

#include "Attr.hpp"

#include "BDDDict.hpp"
#include "FactoredSynthesizer.hpp"

#include "Logger.h"
#include "SymbolicDFA.h"

class DFAGameSolver
{
public:
  DFAGameSolver(SyftMgr m, FactoredSynthesizer s);
  
  bool realizability(const std::vector<SymbolicDFA>& dfas,
                     const std::unique_ptr<Logger>& logger) const;
	
  /*my::optional<std::unordered_map<unsigned int, BDD>> realizablity_variant(
    const SymbolicDFA& dfa);*/
  
  virtual ~DFAGameSolver();
  
  /* void printBDDSat(const BDD& b, const SymbolicDFA& dfa);*/

private:
  SyftMgr mgr;
  FactoredSynthesizer synthesizer;

  size_t node_count(const std::vector<SymbolicDFA>& dfa) const;
  size_t node_count(const std::vector<SkolemFunction>& strategy) const;
  
  bool reached_fixpoint(const std::vector<BDD>& winning_states) const;
  BDD prime(const BDD& states) const;
  BDD for_all(const jet::AttrSet& vars, const BDD& b) const;
  SynthesisResult one_step_synthesis(const std::vector<SymbolicDFA>& dfas,
                                     const BDD& next_winning_states) const;

        /*
        std::string state2bin(int n);
        BDD state2bdd(int s, const SymbolicDFA& dfa);
	std::vector<int> state2bit(int n, const SymbolicDFA& dfa);
	std::vector<std::vector<int>> outindex(const SymbolicDFA& dfa);
        void dumpdot(BDD &b, std::string filename);
        BDD prime(const BDD& orign, const SymbolicDFA& dfa);
        BDD univsyn(const BDD& winning_states, const SymbolicDFA& dfa);
        BDD existsyn(const BDD& master_plan, const SymbolicDFA& dfa);
        BDD univsyn_invariant(const BDD& univ, const BDD& winning_states);
        BDD existsyn_invariant(const BDD& exist,
			       BDD& transducer,
			       const BDD& winning_states,
			       const SymbolicDFA& dfa);
        void strategy(std::vector<BDD>& S2O, const SymbolicDFA& dfa);
        */
};

#endif // DFAGAMESOLVER_H
