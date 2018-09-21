#ifndef SYN_H
#define SYN_H

#include <memory>

#include "Common.h"
#include "DFA.h"
#include "InputFirst.h"

class syn
{
    public:
        syn(std::shared_ptr<Cudd> m, std::string filename, std::string partfile);
	syn(std::shared_ptr<Cudd> m, std::unique_ptr<DFA> d);
        bool realizablity(std::unordered_map<unsigned int, BDD>& IFstrategy);
        bool realizablity_variant(std::unordered_map<unsigned int, BDD>& IFstrategy);
        virtual ~syn();
	void printBDDSat(BDD b);

	std::unique_ptr<DFA> bdd;

 protected:
    private:
        std::shared_ptr<Cudd> mgr;
        int cur = 0;
        bool fixpoint();
        std::vector<BDD> W;
        std::vector<BDD> Wprime;
        std::string state2bin(int n);
        void initializer();
        BDD state2bdd(int s);
        int* state2bit(int n);
        int** outindex();
        void dumpdot(BDD &b, std::string filename);
        BDD prime(BDD orign);
        BDD univsyn();
        BDD existsyn();
        BDD univsyn_invariant(BDD univ);
        BDD existsyn_invariant(BDD exist, BDD& transducer);
        void strategy(std::vector<BDD>& S2O);
};

#endif // SYN_H
