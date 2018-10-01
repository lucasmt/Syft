#include "DFAGameSolver.h"

#include <memory>

using std::string;
using std::unordered_map;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;
using std::move;

DFAGameSolver::DFAGameSolver(shared_ptr<Cudd> m)
    : mgr(move(m))
{}

DFAGameSolver::~DFAGameSolver()
{}

BDD DFAGameSolver::state2bdd(int s, const DFA& dfa){
    string bin = state2bin(s);
    BDD b = mgr->bddOne();
    int nzero = dfa.nbits - bin.length();

    for(int i = 0; i < nzero; i++){
        b *= !dfa.bddvars[i];
    }
    for(int i = 0; i < bin.length(); i++){
        if(bin[i] == '0')
            b *= !dfa.bddvars[i+nzero];
        else
            b *= dfa.bddvars[i+nzero];
    }
    return b;

}

string DFAGameSolver::state2bin(int n){
    string res;
    while (n)
    {
        res.push_back((n & 1) + '0');
        n >>= 1;
    }

    if (res.empty())
        res = "0";
    else
        reverse(res.begin(), res.end());
   return res;
}

bool DFAGameSolver::reached_fixpoint(const vector<BDD>& winning_states){
    size_t last = winning_states.size() - 1;
    
    return winning_states[last] == winning_states[last - 1];
}

void DFAGameSolver::printBDDSat(const BDD& b, const DFA& dfa){

  std::cout<<"sat with: ";
  int max = dfa.nstates;
  
  for (int i=0; i<max; i++){
      if (b.Eval(state2bit(i, dfa).data()).IsOne()){
	  std::cout<<i<<", ";
    }
  }
  std::cout<<std::endl;
}

my::optional<unordered_map<unsigned int, BDD>> DFAGameSolver::realizablity(
    const DFA& dfa){
    vector<BDD> master_plan(1, dfa.finalstatesBDD);
    vector<BDD> winning_states(1, dfa.finalstatesBDD);
    
    do {
	BDD new_plan = master_plan.back() + univsyn(winning_states.back(), dfa);
	master_plan.push_back(new_plan);

	BDD new_winning = existsyn(master_plan.back(), dfa);
	winning_states.push_back(new_winning);
    }
    while (!reached_fixpoint(winning_states));

    vector<int> initial_state = dfa.initbv;
    
    if(winning_states.back().Eval(initial_state.data()).IsOne()){
        BDD O = mgr->bddOne();

        for(int i = 0; i < dfa.output.size(); i++){

            O *= dfa.bddvars[dfa.output[i]];
        }
	
        InputFirstSynthesis IFsyn(*mgr);

	return IFsyn.synthesize(master_plan.back(), O);
    }

    std::cout << "unrealizable, winning set: " << std::endl;
    std::cout << winning_states.back() << std::endl;

    return my::nullopt;
}

my::optional<unordered_map<unsigned int, BDD>> DFAGameSolver::realizablity_variant(const DFA& dfa){
    BDD transducer;
    vector<BDD> master_plan(1, dfa.finalstatesBDD);
    vector<BDD> winning_states(1, dfa.finalstatesBDD);
    vector<int> initial_state = dfa.initbv;
    
    while(true){
        int index;
        BDD O = mgr->bddOne();
        for(int i = 0; i < dfa.output.size(); i++){
            index = dfa.output[i];
            O *= dfa.bddvars[index];
        }

	BDD new_plan = existsyn_invariant(O, transducer,
					  winning_states.back(), dfa);
	master_plan.push_back(new_plan);

        if(reached_fixpoint(winning_states))
            break;

        BDD I = mgr->bddOne();
        for(int i = 0; i < dfa.input.size(); i++){
            index = dfa.input[i];
            I *= dfa.bddvars[index];
        }

        winning_states.push_back(univsyn_invariant(I, winning_states.back()));

        if((winning_states.back().Eval(initial_state.data())).IsOne()){
            return unordered_map<unsigned int, BDD>();
        }

    }

    if((winning_states.back().Eval(initial_state.data())).IsOne()){
      // TODO: use ifstrategysynthesis
        BDD O = mgr->bddOne();
	vector<BDD> S2O;
        for(int i = 0; i < dfa.output.size(); i++){
            O *= dfa.bddvars[dfa.output[i]];
        }
        O *= dfa.bddvars[dfa.nbits];
        //naive synthesis
	int* outindex;
        transducer.SolveEqn(O, S2O, &outindex, dfa.output.size());
        strategy(S2O, dfa);

        return unordered_map<unsigned int, BDD>();
    }
    return my::nullopt;

}


void DFAGameSolver::strategy(vector<BDD>& S2O, const DFA& dfa){
    vector<BDD> winning;
    for(int i = 0; i < S2O.size(); i++){

        for(int j = 0; j < dfa.output.size(); j++){
            int index = dfa.output[j];
            S2O[i] = S2O[i].Compose(dfa.bddvars[index], mgr->bddOne());
        }
    }
}

vector<vector<int>> DFAGameSolver::outindex(const DFA& dfa){
    int outlength = dfa.output.size();
    int outwidth = 2;
    vector<vector<int>> out(outlength, vector<int>(outwidth));

    for(int l = 0; l < outlength; l++){
        out[l][0] = l;
        out[l][1] = dfa.output[l];
    }
    return out;
}

vector<int> DFAGameSolver::state2bit(int n, const DFA& dfa){
    vector<int> s(dfa.nbits);
    for (int i=dfa.nbits-1; i>=0; i--){
      s[i] = n%2;
      n = n/2;
    }
    return s;
}

BDD DFAGameSolver::univsyn(const BDD& winning_states, const DFA& dfa){
    BDD I = mgr->bddOne();
    BDD tmp = winning_states;
    int index;
    int offset = dfa.nbits + dfa.nvars;
    for(int i = 0; i < dfa.input.size(); i++){
        index = dfa.input[i];
        I *= dfa.bddvars[index];
    }

    tmp = prime(tmp, dfa);

    for(int i = 0; i < dfa.nbits; i++){
        tmp = tmp.Compose(dfa.res[i], offset+i);
    }

    tmp *= !winning_states;

    BDD eliminput = tmp.UnivAbstract(I);

    return eliminput;
}

BDD DFAGameSolver::existsyn_invariant(const BDD& exist,
				      BDD& transducer,
				      const BDD& winning_states,
				      const DFA& dfa){
    BDD tmp = winning_states;
    int offset = dfa.nbits + dfa.nvars;

    tmp = prime(tmp, dfa);
    for(int i = 0; i < dfa.nbits; i++){
        tmp = tmp.Compose(dfa.res[i], offset+i);
    }
    transducer = tmp;
    tmp *= !winning_states;
    BDD elimoutput = tmp.ExistAbstract(exist);
    return elimoutput;
}

BDD DFAGameSolver::univsyn_invariant(const BDD& univ,
				     const BDD& winning_states){

    BDD tmp = winning_states;
    BDD elimuniv = tmp.UnivAbstract(univ);
    return elimuniv;

}

BDD DFAGameSolver::prime(const BDD& orign, const DFA& dfa){
    int offset = dfa.nbits + dfa.nvars;
    BDD tmp = orign;
    for(int i = 0; i < dfa.nbits; i++){
        tmp = tmp.Compose(dfa.bddvars[i+offset], i);
    }
    return tmp;
}

BDD DFAGameSolver::existsyn(const BDD& master_plan, const DFA& dfa){
    BDD O = mgr->bddOne();
    BDD tmp = master_plan;
    int index;
    int offset = dfa.nbits + dfa.nvars;
    for(int i = 0; i < dfa.output.size(); i++){
        index = dfa.output[i];
        O *= dfa.bddvars[index];
    }
    BDD elimoutput = tmp.ExistAbstract(O);
    return elimoutput;
}

void DFAGameSolver::dumpdot(BDD &b, string filename){
    FILE *fp = fopen(filename.c_str(), "w");
    vector<BDD> single(1);
    single[0] = b;
	this->mgr->DumpDot(single, NULL, NULL, fp);
	fclose(fp);
}
