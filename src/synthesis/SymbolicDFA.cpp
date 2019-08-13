#include "SymbolicDFA.h"

#include "debug.h"

using std::vector;

SymbolicDFA::SymbolicDFA(const DFA& dfa, const SyftMgr& mgr)
{
  _index = dfa.index();
  
  _env_vars = dfa.env_vars();
  _sys_vars = dfa.sys_vars();
  _state_vars = mgr.state_map.state_vars(dfa);
  _next_state_vars = mgr.state_map.next_state_vars(dfa);

  _input_vars = _env_vars.unionWith(_state_vars);
  _output_vars = _sys_vars.unionWith(_next_state_vars);

  _initial_assignment = mgr.state_map.encode_current(dfa.initial_state());

  _transition_relation = construct_bdd_new(dfa, mgr);

  _accepting_states = mgr.cudd_mgr.bddZero();

  for (DFAState accepting_state : dfa.accepting_states())
  {
    _accepting_states |= mgr.minterm(accepting_state);
  }
}

size_t SymbolicDFA::index() const { return _index; }

jet::AttrSet SymbolicDFA::env_vars() const { return _env_vars; }
jet::AttrSet SymbolicDFA::sys_vars() const { return _sys_vars; }
jet::AttrSet SymbolicDFA::state_vars() const { return _state_vars; }
jet::AttrSet SymbolicDFA::next_state_vars() const { return _next_state_vars; }
jet::AttrSet SymbolicDFA::input_vars() const { return _input_vars; }
jet::AttrSet SymbolicDFA::output_vars() const { return _output_vars; }

Assignment SymbolicDFA::initial_assignment() const
{
  return _initial_assignment;
}

BDD SymbolicDFA::transition_relation() const
{
  return _transition_relation;
}

BDD SymbolicDFA::accepting_states() const
{
  return _accepting_states;
}

BDD SymbolicDFA::construct_bdd_new(const DFA& dfa, const SyftMgr& mgr) const
{
  vector<ADD> adds = mgr.interpret(dfa.transition_function());
  BDD bdd = mgr.cudd_mgr.bddZero();

  vector<BDD> state_minterms;
  state_minterms.reserve(dfa.number_of_states());
  
  for (size_t i = 0; i < dfa.number_of_states(); i++)
  {
    DFAState state = dfa.ith_state(i);
    BDD state_minterm = mgr.minterm(state);
    state_minterms.push_back(state_minterm);
  }
  
  for (size_t i = 0; i < dfa.number_of_states(); i++)
  {
    BDD state_transition = state_minterms[i];
    
    for (size_t j = 0; j < mgr.state_map.vars_per_dfa(dfa); j++)
    {
      jet::Attr next_state_var = mgr.state_map.next_state_var(dfa, j);
      BDD next_state_var_bdd = mgr.bdd_dict->bddOfVar(next_state_var);
      
      state_transition &= adds[i].BddIthBit(j).Xnor(next_state_var_bdd);
    }
    
    bdd |= state_transition;
  }

  return bdd;
  /*
  BDD transition_relation = bdd_dict->bddZero();

  vector<vector<BDD>> tBDD(dfa.smtbdd.size());

  for(size_t i = 0; i < tBDD.size(); i++){
    if(tBDD[i].size() == 0){
      vector<BDD> b = try_get(i, tBDD, dfa.smtbdd,
			      symbolic_dfa.number_of_bits,
			      symbolic_dfa.bdd_vars);
    }
  }

  for (jet::Attr state_var : state_vars){
    for (DFAState state : dfa.states()){
      
      BDD tmp = bdd_dict->bddOne();
      
  
  for(size_t i = 0; i < symbolic_dfa.number_of_bits; i++){
    for(size_t j = 0; j < dfa.number_of_states; j++){
      BDD tmp = mgr->bddOne();
      string bins = state2bin(j);
      size_t offset = symbolic_dfa.number_of_bits - bins.size();
      for(size_t m = 0; m < offset; m++){
	tmp = tmp & var2bddvar(0, m, symbolic_dfa.bdd_vars);
      }
      for(size_t m = 0; m < bins.size(); m++){
	tmp = tmp & var2bddvar(int(bins[m])-48, m + offset,
			       symbolic_dfa.bdd_vars);
      }
      tmp = tmp & tBDD[dfa.behaviour[j]][i];

      symbolic_dfa.transition_function[i] |= tmp;
    }
  }
  */
}

vector<BDD> SymbolicDFA::try_get(size_t index,
                                 vector<vector<BDD>>& tBDD,
                                 const vector<vector<size_t>>& smtbdd,
                                 size_t nbits,
                                 const vector<BDD>& bdd_vars) const
{
  return vector<BDD>();
  /*
  if(tBDD[index].size() != 0)
    return tBDD[index];
  vector<BDD> b;
  if(smtbdd[index][0] == -1){
    int s = smtbdd[index][1];
    string bins = state2bin(s);
    for(int m = 0; m < nbits - bins.size(); m++){
      b.push_back(mgr->bddZero());
    }
    for(int i = 0; i < bins.size(); i++){
      if(bins[i] == '0')
	b.push_back(mgr->bddZero());
      else if(bins[i] == '1')
	b.push_back(mgr->bddOne());
      else
	cout<<"error binary state"<<endl;
    }
    tBDD[index] = b;
    return b;
  }
  else{
    int rootindex = smtbdd[index][0];
    int leftindex = smtbdd[index][1];
    int rightindex = smtbdd[index][2];
    BDD root = bdd_vars[rootindex+nbits];
    //dumpdot(root, "test");
    vector<BDD> left = try_get(leftindex, tBDD, smtbdd, nbits, bdd_vars);
    //for(int l = 0; l < left.size(); l++)
    // dumpdot(left[l], "left"+to_string(l));
    vector<BDD> right = try_get(rightindex, tBDD, smtbdd, nbits, bdd_vars);
    //for(int l = 0; l < left.size(); l++)
    // dumpdot(right[l], "right"+to_string(l));
    assert(left.size() == right.size());
    for(int i = 0; i < left.size(); i++){
      BDD tmp;
      tmp = root.Ite(right[i], left[i]);//Assume this is correct
      //dumpdot(tmp, "tmp");
      b.push_back(tmp);
    }
    tBDD[index] = b;
    return b;
  }
  */
}
