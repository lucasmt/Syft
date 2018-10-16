#ifndef DFAGAMESOLVER_H
#define DFAGAMESOLVER_H

#include <memory>

#include "Common.h"
#include "SymbolicDFA.h"
#include "InputFirst.h"
#include "optional.h"

class DFAGameSolver
{
public:
    DFAGameSolver(std::shared_ptr<Cudd> m);

    my::optional<std::unordered_map<unsigned int, BDD>> realizablity(
	const SymbolicDFA& dfa);
	
    my::optional<std::unordered_map<unsigned int, BDD>> realizablity_variant(
	const SymbolicDFA& dfa);
	
    virtual ~DFAGameSolver();

    void printBDDSat(const BDD& b, const SymbolicDFA& dfa);

private:
        std::shared_ptr<Cudd> mgr;
    
        bool reached_fixpoint(const std::vector<BDD>& winning_states);
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
};

#endif // DFAGAMESOLVER_H
