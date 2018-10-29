#include "DFAGameSolver.h"

#include <memory>

#include "SkolemFunction.hpp"
#include "SynthesisResult.hpp"

using std::string;
using std::unordered_map;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;
using std::move;

DFAGameSolver::DFAGameSolver(shared_ptr<BDDMgr> m,
                             BoolesMethod s)
    : mgr(move(m))
    , synthesizer(move(s))
{}

DFAGameSolver::~DFAGameSolver()
{}

BDD DFAGameSolver::state2bdd(int s, const SymbolicDFA& dfa){
    string bin = state2bin(s);
    BDD b = mgr->bddOne();
    int nzero = dfa.number_of_bits - bin.length();

    for(int i = 0; i < nzero; i++){
        b *= !dfa.bdd_vars[i];
    }
    for(int i = 0; i < bin.length(); i++){
        if(bin[i] == '0')
            b *= !dfa.bdd_vars[i+nzero];
        else
            b *= dfa.bdd_vars[i+nzero];
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

void DFAGameSolver::printBDDSat(const BDD& b, const SymbolicDFA& dfa){
  std::cout<<"sat with: ";
  int max = dfa.number_of_states;
  
  for (int i=0; i<max; i++){
      if (b.Eval(state2bit(i, dfa).data()).IsOne()){
	  std::cout<<i<<", ";
    }
  }
  std::cout<<std::endl;
}

bool DFAGameSolver::realizablity(const SymbolicDFA& dfa){

  vector<SkolemFunction> strategy(1, mgr->bddOne());
  vector<BDD> winning_states(1, dfa.accepting_states());
    
  do {
    SynthesisResult result =
      one_step_synthesis(dfa, prime(winning_states.back()));
        
    BDD new_winning = for_all(dfa.env_vars(), result.precondition);
    winning_states.push_back(new_winning);
    strategy.push_back(result.skolemFunction);
  }
  while (!reached_fixpoint(winning_states));

  Assignment initial_state = dfa.initial_assignment();
    
  return mgr->eval(winning_states.back(), initial_assignment);
}

BDD DFAGameSolver::for_all(const jet::AttrSet& vars, const BDD& b){
  BDD cube = mgr->cubeOfVars(vars);

  return b.UnivAbstract(cube);
}

BDD DFAGameSolver::one_step_synthesis(const SymbolicDFA& dfa,
                                      const BDD& next_winning_states){
  BDD transition = dfa.transition_relation();
  jet::AttrSet output_vars = dfa.output_vars();
    
  return synthesizer->run(transition & next_winning_states, output_vars);
}

/*
my::optional<unordered_map<unsigned int, BDD>> DFAGameSolver::realizablity(
    const SymbolicDFA& dfa){
    vector<BDD> master_plan(1, dfa.final_states);
    vector<BDD> winning_states(1, dfa.final_states);
    
    do {
	BDD new_plan = master_plan.back() + univsyn(winning_states.back(), dfa);
	master_plan.push_back(new_plan);

	BDD new_winning = existsyn(master_plan.back(), dfa);
	winning_states.push_back(new_winning);
    }
    while (!reached_fixpoint(winning_states));

    vector<int> initial_state = dfa.initial_bitvector;
    
    if(winning_states.back().Eval(initial_state.data()).IsOne()){
        BDD O = mgr->bddOne();

        for(int i = 0; i < dfa.output_indices.size(); i++){

            O *= dfa.bdd_vars[dfa.output_indices[i]];
        }
	
        InputFirstSynthesis IFsyn(mgr->cuddMgr());

	return IFsyn.synthesize(master_plan.back(), O);
    }

    std::cout << "unrealizable, winning set: " << std::endl;
    std::cout << winning_states.back() << std::endl;

    return my::nullopt;
}

my::optional<unordered_map<unsigned int, BDD>> DFAGameSolver::realizablity_variant(const SymbolicDFA& dfa){
    BDD transducer;
    vector<BDD> master_plan(1, dfa.final_states);
    vector<BDD> winning_states(1, dfa.final_states);
    vector<int> initial_state = dfa.initial_bitvector;
    
    while(true){
        int index;
        BDD O = mgr->bddOne();
        for(int i = 0; i < dfa.output_indices.size(); i++){
            index = dfa.output_indices[i];
            O *= dfa.bdd_vars[index];
        }

	BDD new_plan = existsyn_invariant(O, transducer,
					  winning_states.back(), dfa);
	master_plan.push_back(new_plan);

        if(reached_fixpoint(winning_states))
            break;

        BDD I = mgr->bddOne();
        for(int i = 0; i < dfa.input_indices.size(); i++){
            index = dfa.input_indices[i];
            I *= dfa.bdd_vars[index];
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
        for(int i = 0; i < dfa.output_indices.size(); i++){
            O *= dfa.bdd_vars[dfa.output_indices[i]];
        }
        O *= dfa.bdd_vars[dfa.number_of_bits];
        //naive synthesis
	int* outindex;
        transducer.SolveEqn(O, S2O, &outindex, dfa.output_indices.size());
        strategy(S2O, dfa);

        return unordered_map<unsigned int, BDD>();
    }
    return my::nullopt;

}


void DFAGameSolver::strategy(vector<BDD>& S2O, const SymbolicDFA& dfa){
    vector<BDD> winning;
    for(int i = 0; i < S2O.size(); i++){

        for(int j = 0; j < dfa.output_indices.size(); j++){
            int index = dfa.output_indices[j];
            S2O[i] = S2O[i].Compose(dfa.bdd_vars[index], mgr->bddOne());
        }
    }
}


vector<vector<int>> DFAGameSolver::outindex(const SymbolicDFA& dfa){
    int outlength = dfa.output_indices.size();
    int outwidth = 2;
    vector<vector<int>> out(outlength, vector<int>(outwidth));

    for(int l = 0; l < outlength; l++){
        out[l][0] = l;
        out[l][1] = dfa.output_indices[l];
    }
    return out;
}

vector<int> DFAGameSolver::state2bit(int n, const SymbolicDFA& dfa){
    vector<int> s(dfa.number_of_bits);
    for (int i=dfa.number_of_bits-1; i>=0; i--){
      s[i] = n%2;
      n = n/2;
    }
    return s;
}

BDD DFAGameSolver::univsyn(const BDD& winning_states, const SymbolicDFA& dfa){
    BDD I = mgr->bddOne();
    BDD tmp = winning_states;
    int index;
    int offset = dfa.number_of_bits + dfa.number_of_vars;
    for(int i = 0; i < dfa.input_indices.size(); i++){
        index = dfa.input_indices[i];
        I *= dfa.bdd_vars[index];
    }

    tmp = prime(tmp, dfa);

    for(int i = 0; i < dfa.number_of_bits; i++){
        tmp = tmp.Compose(dfa.transition_function[i], offset+i);
    }

    tmp *= !winning_states;

    BDD eliminput = tmp.UnivAbstract(I);

    return eliminput;
}

BDD DFAGameSolver::existsyn_invariant(const BDD& exist,
				      BDD& transducer,
				      const BDD& winning_states,
				      const SymbolicDFA& dfa){
    BDD tmp = winning_states;
    int offset = dfa.number_of_bits + dfa.number_of_vars;

    tmp = prime(tmp, dfa);
    for(int i = 0; i < dfa.number_of_bits; i++){
        tmp = tmp.Compose(dfa.transition_function[i], offset+i);
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

BDD DFAGameSolver::prime(const BDD& orign, const SymbolicDFA& dfa){
    int offset = dfa.number_of_bits + dfa.number_of_vars;
    BDD tmp = orign;
    for(int i = 0; i < dfa.number_of_bits; i++){
        tmp = tmp.Compose(dfa.bdd_vars[i+offset], i);
    }
    return tmp;
}

BDD DFAGameSolver::existsyn(const BDD& master_plan, const SymbolicDFA& dfa){
    BDD O = mgr->bddOne();
    BDD tmp = master_plan;
    int index;
    int offset = dfa.number_of_bits + dfa.number_of_vars;
    for(int i = 0; i < dfa.output_indices.size(); i++){
        index = dfa.output_indices[i];
        O *= dfa.bdd_vars[index];
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
*/
