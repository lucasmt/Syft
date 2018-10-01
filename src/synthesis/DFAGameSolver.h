#ifndef DFAGAMESOLVER_H
#define DFAGAMESOLVER_H

#include <memory>

#include "Common.h"
#include "DFA.h"
#include "InputFirst.h"
#include "optional.h"

class DFAGameSolver
{
public:
    DFAGameSolver(std::shared_ptr<Cudd> m);

    my::optional<std::unordered_map<unsigned int, BDD>> realizablity(
	const DFA& dfa);
	
    my::optional<std::unordered_map<unsigned int, BDD>> realizablity_variant(
	const DFA& dfa);
	
    virtual ~DFAGameSolver();

    void printBDDSat(const BDD& b, const DFA& dfa);

private:
        std::shared_ptr<Cudd> mgr;
    
        bool reached_fixpoint(const std::vector<BDD>& winning_states);
        std::string state2bin(int n);
        BDD state2bdd(int s, const DFA& dfa);
	std::vector<int> state2bit(int n, const DFA& dfa);
	std::vector<std::vector<int>> outindex(const DFA& dfa);
        void dumpdot(BDD &b, std::string filename);
        BDD prime(const BDD& orign, const DFA& dfa);
        BDD univsyn(const BDD& winning_states, const DFA& dfa);
        BDD existsyn(const BDD& master_plan, const DFA& dfa);
        BDD univsyn_invariant(const BDD& univ, const BDD& winning_states);
        BDD existsyn_invariant(const BDD& exist,
			       BDD& transducer,
			       const BDD& winning_states,
			       const DFA& dfa);
        void strategy(std::vector<BDD>& S2O, const DFA& dfa);
};

#endif // DFAGAMESOLVER_H
