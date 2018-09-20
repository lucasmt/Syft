#ifndef SYN_H
#define SYN_H
#include "Common.h"
#include "DFA.h"
#include "InputFirst.h"

class syn
{
    public:
        syn(Cudd* m, std::string filename, std::string partfile);
	syn(Cudd* m, DFA* d);
	bool realizablity_sys(std::unordered_map<unsigned int, BDD>& IFstrategy);
        bool realizablity_env(std::unordered_map<unsigned int, BDD>& IFstrategy);
        virtual ~syn();
	void printBDDSat(BDD b);

	DFA* bdd;

 protected:
    private:
        Cudd* mgr;
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
        BDD univsyn_sys(BDD univ);
        BDD existsyn_sys(BDD exist);
        BDD univsyn_env(BDD univ);
        BDD existsyn_env(BDD exist, BDD& transducer);
        void strategy(std::vector<BDD>& S2O);
};

#endif // SYN_H
