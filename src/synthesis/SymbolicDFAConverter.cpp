#include "SymbolicDFAConverter.h"

#include <iostream>

#include <boost/algorithm/string.hpp>

#include "BDDMgr.hpp"

using std::cout;
using std::endl;
using std::move;
using std::shared_ptr;
using std::string;
using std::vector;

SymbolicDFAConverter::SymbolicDFAConverter(shared_ptr<BDDMgr> m)
  : mgr(move(m))
{}

SymbolicDFA SymbolicDFAConverter::run(const DFA& dfa)
{
  SymbolicDFA symbolic_dfa;
  symbolic_dfa.number_of_vars = dfa.number_of_vars;
  symbolic_dfa.number_of_states = dfa.number_of_states;
  symbolic_dfa.number_of_bits = state2bin(dfa.number_of_states - 1).length();

  symbolic_dfa.initial_bitvector = vector<int>(symbolic_dfa.number_of_bits);
  size_t temp = dfa.initial_state;

  // Weird loop for backwards iteration with unsigned index
  for (size_t i = symbolic_dfa.number_of_bits; i-- > 0;) {
    symbolic_dfa.initial_bitvector[i] = temp%2;
    temp = temp/2;
  }

  for(size_t i = 1; i < dfa.var_names.size(); i++){
    if(dfa.partition.input_vars.find(dfa.var_names[i]) != dfa.partition.input_vars.end())
      symbolic_dfa.input_indices.push_back(symbolic_dfa.number_of_bits+i-1);
    else if(dfa.partition.output_vars.find(dfa.var_names[i]) != dfa.partition.output_vars.end())
      symbolic_dfa.output_indices.push_back(symbolic_dfa.number_of_bits+i-1);
    else if(dfa.var_names[i] == "ALIVE")
      symbolic_dfa.output_indices.push_back(symbolic_dfa.number_of_bits+i-1);
    else
      cout<<"error: "<<dfa.var_names[i]<<endl;
  }
  
  construct_bdd_new(symbolic_dfa, dfa);
  
  // Primed state variables?
  size_t initial_id = symbolic_dfa.number_of_bits + dfa.number_of_vars;
  for(size_t i = 0; i < symbolic_dfa.number_of_bits; i++){
    jet::Attr var(initial_id + i);
    BDD b = mgr->bddOfVar(var);
    symbolic_dfa.bdd_vars.push_back(b);
  }

  return symbolic_dfa;
}


string SymbolicDFAConverter::state2bin(int n) const {
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
  //cout<<res<<endl;
  return res;
}

void SymbolicDFAConverter::construct_bdd_new(SymbolicDFA& symbolic_dfa,
                                             const DFA& dfa) const {
  for(size_t i = 0; i < symbolic_dfa.number_of_bits+dfa.number_of_vars; i++){
    jet::Attr var(i);
    BDD b = mgr->bddOfVar(var);
    symbolic_dfa.bdd_vars.push_back(b);
    //dumpdot(b, to_string(i));
  }
  
  for(size_t i = 0; i < symbolic_dfa.number_of_bits; i++){
    jet::Attr var(i);
    BDD d = mgr->bddZero();
    symbolic_dfa.transition_function.push_back(d);
  }

  vector<vector<BDD>> tBDD(dfa.smtbdd.size());
  
  for(size_t i = 0; i < tBDD.size(); i++){
    if(tBDD[i].size() == 0){
      vector<BDD> b = try_get(i, tBDD, dfa.smtbdd,
                              symbolic_dfa.number_of_bits,
                              symbolic_dfa.bdd_vars);
    }
  }

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

  symbolic_dfa.final_states = mgr->bddZero();
  for(size_t i = 0; i < dfa.final_states.size(); i++){
    BDD ac = state2bdd(dfa.final_states[i],
                       symbolic_dfa.number_of_bits,
                       symbolic_dfa.bdd_vars);
    symbolic_dfa.final_states |= ac;
  }
}
  
vector<BDD> SymbolicDFAConverter::try_get(
  size_t index,
  vector<vector<BDD>>& tBDD,
  const vector<vector<size_t>>& smtbdd,
  size_t nbits,
  const vector<BDD>& bdd_vars) const
{
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
}

BDD SymbolicDFAConverter::var2bddvar(int v,
                                     int index,
                                     const vector<BDD>& bdd_vars) const {
  if(v == 0){
    return !bdd_vars[index];
  }
  else{
    return bdd_vars[index];
  }
}

BDD SymbolicDFAConverter::state2bdd(size_t s,
                                    size_t nbits,
                                    const vector<BDD>& bdd_vars) const {
  string bin = state2bin(s);
  BDD b = mgr->bddOne();
  int nzero = nbits - bin.length();
  //cout<<nzero<<endl;
  for(int i = 0; i < nzero; i++){
    b *= !bdd_vars[i];
  }
  for(int i = 0; i < bin.length(); i++){
    if(bin[i] == '0')
      b *= !bdd_vars[i+nzero];
    else
      b *= bdd_vars[i+nzero];
  }
  return b;

}
