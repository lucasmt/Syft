#include "DFAGameSolver.h"

#include <memory>
#include <random>

#include "SkolemFunction.hpp"
#include "SynthesisResult.hpp"

using std::string;
using std::unordered_map;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;
using std::move;

#include <iostream>

void print_strategy(const vector<SkolemFunction>& strategy, const SyftMgr& m)
{
  for (size_t i = 0; i < strategy.size(); i++)
  {
    for (jet::Attr var : m.output_vars())
    {
      std::cout << i << ": "
                << m.bdd_dict->bddOfVar(var) << " = "
                << strategy[i][var] << std::endl;
    }
  }
}

void print_winning_states(const BDD& bdd)
{
  std::cout << "Winning states: " << bdd << std::endl;
}

void print_dfa(const SymbolicDFA& dfa, const SyftMgr& m)
{
  jet::AttrSet env_vars = dfa.env_vars();
  jet::AttrSet sys_vars = dfa.sys_vars();
  jet::AttrSet state_vars = dfa.input_vars().differenceWith(env_vars);
  jet::AttrSet next_state_vars = dfa.output_vars().differenceWith(sys_vars);
  
  std::cout << "State vars: "
            << state_vars.toStringWith("z")
            << " or "
            << m.bdd_dict->cubeOfVars(state_vars)
            << std::endl;

  std::cout << "Environment vars: "
            << env_vars.toStringWith("x")
            << " or "
            << m.bdd_dict->cubeOfVars(env_vars)
            << std::endl;
  
  std::cout << "System vars: "
            << sys_vars.toStringWith("y")
            << " or "
            << m.bdd_dict->cubeOfVars(sys_vars)
            << std::endl;

  std::cout << "Next-State vars: "
            << next_state_vars.toStringWith("z'")
            << " or "
            << m.bdd_dict->cubeOfVars(next_state_vars)
            << std::endl;

  std::cout << "Initial state: "
            << m. minterm(dfa.index(), dfa.initial_assignment())
            << std::endl;
  
  std::cout << "Accepting states: " << dfa.accepting_states() << std::endl;
}

Assignment random_assignment(const jet::AttrSet& vars,
                             std::mt19937& rng,
                             std::bernoulli_distribution& dist)
{
  vector<jet::Attr> assigned_to_true;

  for (jet::Attr var : vars)
  {
    if (dist(rng))
      assigned_to_true.push_back(var);
  }

  return Assignment(jet::AttrSet(assigned_to_true));
}

void print_example_play(const vector<SkolemFunction>& strategy,
                        const SyftMgr& mgr,
                        const Assignment& initial_state)
{
  std::mt19937 rng;
  rng.seed(std::random_device()());
  std::bernoulli_distribution dist(0.5);

  Assignment assignment = initial_state;

  std::cout << "Initial state: "
            << assignment.assignedToTrue().toStringWith("z")
            << std::endl;
  
  for (auto it = strategy.rbegin(); it != strategy.rend(); ++it)
  {
    SkolemFunction one_step_strategy = *it;
    Assignment env_assignment = random_assignment(mgr.var_partition.env_vars(),
                                                  rng,
                                                  dist);

    std::cout << "Input: "
              << env_assignment.assignedToTrue().toStringWith("x")
              << std::endl;
    
    assignment &= env_assignment;
    Assignment output_assignment = one_step_strategy(assignment);

    std::cout << "Output: " <<
      output_assignment
      .assignedToTrue()
      .intersectWith(mgr.var_partition.sys_vars())
      .toStringWith("y") << std::endl;
    
    jet::AttrSet assigned_to_true =
      output_assignment
      .assignedToTrue()
      .differenceWith(mgr.var_partition.sys_vars());
    
    assignment = Assignment(mgr.state_map.unprime(assigned_to_true));

    std::cout << "Next state: "
              << assignment.assignedToTrue().toStringWith("z")
              << std::endl;
  }
}

DFAGameSolver::DFAGameSolver(SyftMgr m, FactoredSynthesizer s)
  : mgr(move(m))
  , synthesizer(move(s))
{}

DFAGameSolver::~DFAGameSolver()
{}

/*
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
*/

bool DFAGameSolver::reached_fixpoint(const vector<BDD>& winning_states) const {
    size_t last = winning_states.size() - 1;
    
    return winning_states[last] == winning_states[last - 1];
}

/*
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
*/

BDD DFAGameSolver::prime(const BDD& states) const
{
  BDD primed_states = states;
  
  for (unsigned int index : states.SupportIndices())
  {
    jet::Attr var = mgr.bdd_dict->varAtIndex(index);
    jet::Attr primed_var = mgr.state_map.prime(var);
    BDD bdd_var = mgr.bdd_dict->bddOfVar(primed_var);
    primed_states = primed_states.Compose(bdd_var, index);
  }

  return primed_states;
}

bool DFAGameSolver::realizability(const vector<SymbolicDFA>& dfas) const {

  vector<SkolemFunction> strategy;

  Assignment initial_assignment;
  vector<BDD> winning_states(1, mgr.cudd_mgr.bddOne());

  // Initial set of winning states (maybe can maintain in factored form?)
  for (const SymbolicDFA& dfa : dfas) {
    print_dfa(dfa, mgr);
    initial_assignment &= dfa.initial_assignment();
    winning_states[0] &= dfa.accepting_states();
  }

  print_winning_states(winning_states.back());

  bool reached_initial = false;
  
  do {
    SynthesisResult result =
      one_step_synthesis(dfas, prime(winning_states.back()));
        
    BDD new_winning = for_all(mgr.var_partition.env_vars(),
                              result.precondition);

    BDD old_winning = winning_states.back();
    winning_states.push_back(old_winning | new_winning);
    strategy.push_back(result.skolemFunction);

    print_winning_states(winning_states.back());

    reached_initial = mgr.bdd_dict->eval(winning_states.back(),
                                         initial_assignment);
  }
  while (!reached_fixpoint(winning_states) && !reached_initial);

  if (reached_initial)
  {
    print_strategy(strategy, mgr);

    print_example_play(strategy, mgr, initial_assignment);
  }
  
  return reached_initial;
}

BDD DFAGameSolver::for_all(const jet::AttrSet& vars, const BDD& b) const {
  BDD cube = mgr.bdd_dict->cubeOfVars(vars);

  return b.UnivAbstract(cube);
}

SynthesisResult DFAGameSolver::one_step_synthesis(
  const vector<SymbolicDFA>& dfas,
  const BDD& next_winning_states) const
{
  vector<BDD> factored_preimage;
  factored_preimage.reserve(dfas.size() + 1);

  for (const SymbolicDFA& dfa : dfas)
    factored_preimage.push_back(dfa.transition_relation());

  factored_preimage.push_back(next_winning_states);

  jet::AttrSet next_state_vars = mgr.state_map.next_state_vars();
  jet::AttrSet sys_vars = mgr.var_partition.sys_vars();
  jet::AttrSet output_vars = sys_vars.unionWith(next_state_vars);
    
  return synthesizer.run(factored_preimage, output_vars);
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
