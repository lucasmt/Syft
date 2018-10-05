#include "DFALoader.h"

#include <fstream>
#include <iostream>
#include <set>

#include <boost/algorithm/string.hpp>

using std::cout;
using std::endl;
using std::ifstream;
using boost::algorithm::is_any_of;
using std::move;
using std::set;
using std::shared_ptr;
using boost::algorithm::split;
using std::string;
using std::vector;

DFALoader::DFALoader(shared_ptr<Cudd> m)
  : mgr(move(m))
{}

DFA DFALoader::run(const string& filename, const string& partfile)
{
  DFAArgs args = read_from_file(filename);
  size_t nbits = state2bin(args.nstates-1).length();

  DFABDDs bdds = construct_bdd_new(nbits, args);
  
  IOPartition part = read_partfile(partfile, args.variables, nbits);

  vector<int> initbv = vector<int>(nbits);
  int temp = args.init;
  for (int i=nbits-1; i>=0; i--){
    initbv[i] = temp%2;
    temp = temp/2;
  }
  
  // Primed state variables?
  for(int i = 0; i < nbits; i++){
    BDD b = mgr->bddVar();
    bdds.bddvars.push_back(b);
  }

  return DFA(nbits, move(args), move(bdds), move(initbv), move(part));
}

DFAArgs DFALoader::read_from_file(const string& filename) const {
  ifstream f(filename);
  DFAArgs args;
  
  if(f.is_open()){
    bool flag = 0;
    string line;
    vector<size_t> tmp;
    vector<string> fields; //temporary varibale

    while(getline(f, line)){
      if(flag == 0){
        if(strfind(line, "number of variables")){
          split(fields, line, is_any_of(" "));
          args.nvars = stoi(fields[3]);
          //cout<<nvars<<endl;
        }
        if(strfind(line, "variables") && !strfind(line, "number")){
          split(args.variables, line, is_any_of(" "));

        }
        else if(strfind(line, "states")){
          split(fields, line, is_any_of(" "));
          args.nstates = stoi(fields[1]);
          // cout<<nstates<<endl;
        }
        else if(strfind(line, "initial")){
          split(fields, line, is_any_of(" "));
          args.init = stoi(fields[1]);
          //cout<<init<<endl;
        }
        else if(strfind(line, "bdd nodes")){
          split(fields, line, is_any_of(" "));
          args.nodes = stoi(fields[2]);
          //cout<<nodes<<endl;
        }
        else if(strfind(line, "final")){
          split(fields, line, is_any_of(" "));
          int i = 1; // start at 1 to ignore "final" token
          while(i < fields.size()){
            if(fields[i] == "1")
              args.finalstates.push_back(i-1);
            i = i + 1;
          }
          //print_int(finalstates);
        }
        else if(strfind(line, "behaviour")){
          split(fields, line, is_any_of(" "));
          int i = 1;
          while(i < fields.size()){
            args.behaviour.push_back(stoi(fields[i]));
            i = i + 1;
          }
          //print_int(behaviour);
        }
        else if(strfind(line, "bdd:"))
          flag = 1;
        else
          continue;
      }
      else{
        if(strfind(line, "end"))
          break;
        split(fields, line, is_any_of(" "));
        for(int i = 1; i < fields.size(); i++)
          tmp.push_back(stoi(fields[i]));
        args.smtbdd.push_back(tmp);
        tmp.clear();
      }
    }

  }
  f.close();

  return args;
}

bool DFALoader::strfind(const string& str, const string& target) const {
  size_t found = str.find(target);
  if(found != string::npos)
    return true;
  else
    return false;
}

string DFALoader::state2bin(int n) const {
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

DFABDDs DFALoader::construct_bdd_new(size_t nbits, const DFAArgs& args) const {
  DFABDDs bdds;

  for(int i = 0; i < nbits+args.nvars; i++){
    BDD b = mgr->bddVar();
    bdds.bddvars.push_back(b);
    //dumpdot(b, to_string(i));
  }
  
  for(int i = 0; i < nbits; i++){
    BDD d = mgr->bddZero();
    bdds.res.push_back(d);
  }

  vector<vector<BDD>> tBDD(args.smtbdd.size());
  
  for(int i = 0; i < tBDD.size(); i++){
    if(tBDD[i].size() == 0){
      vector<BDD> b = try_get(i, tBDD, args.smtbdd, nbits, bdds.bddvars);
    }
  }

  for(int i = 0; i < nbits; i++){
    for(int j = 0; j < args.nstates; j++){
      BDD tmp = mgr->bddOne();
      string bins = state2bin(j);
      int offset = nbits - bins.size();
      for(int m = 0; m < offset; m++){
        tmp = tmp * var2bddvar(0, m, bdds.bddvars);
      }
      for(int m = 0; m < bins.size(); m++){
        tmp = tmp * var2bddvar(int(bins[m])-48, m + offset, bdds.bddvars);
      }
      tmp = tmp * tBDD[args.behaviour[j]][i];
      bdds.res[i] = bdds.res[i] + tmp;
    }
  }

  bdds.finalstatesBDD = mgr->bddZero();
  for(int i = 0; i < args.finalstates.size(); i++){
    BDD ac = state2bdd(args.finalstates[i], nbits, bdds.bddvars);
    bdds.finalstatesBDD += ac;
  }

  return bdds;
}
  
vector<BDD> DFALoader::try_get(size_t index,
                               vector<vector<BDD>>& tBDD,
                               const vector<vector<size_t>>& smtbdd,
                               size_t nbits,
                               const vector<BDD>& bddvars) const {
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
    BDD root = bddvars[rootindex+nbits];
    //dumpdot(root, "test");
    vector<BDD> left = try_get(leftindex, tBDD, smtbdd, nbits, bddvars);
    //for(int l = 0; l < left.size(); l++)
    // dumpdot(left[l], "left"+to_string(l));
    vector<BDD> right = try_get(rightindex, tBDD, smtbdd, nbits, bddvars);
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

BDD DFALoader::var2bddvar(int v, int index, const vector<BDD>& bddvars) const {
  if(v == 0){
    return !bddvars[index];
  }
  else{
    return bddvars[index];
  }
}

IOPartition DFALoader::read_partfile(const string& partfile,
                                     const vector<string>& variables,
                                     size_t nbits) const {
  ifstream f(partfile);
  vector<string> inputs;
  vector<string> outputs;
  string line;
  while(getline(f, line)){
    if(f.is_open()){
      if(strfind(line, "inputs")){
        split(inputs, line, is_any_of(" "));
      }
      else if(strfind(line, "outputs")){
        split(outputs, line, is_any_of(" "));
      }
      else{
        cout<<"read partfile error!"<<endl;
        cout<<partfile<<endl;
        cout<<line<<endl;
      }
    }
  }
  f.close();
  set<string> input_set;
  set<string> output_set;
  for(int i = 1; i < inputs.size(); i++)
    input_set.insert(inputs[i]);
  for(int i = 1; i < outputs.size(); i++)
    output_set.insert(outputs[i]);

  IOPartition part;
  
  for(int i = 1; i < variables.size(); i++){
    if(input_set.find(variables[i]) != input_set.end())
      part.input.push_back(nbits+i-1);
    else if(output_set.find(variables[i]) != output_set.end())
      part.output.push_back(nbits+i-1);
    else if(variables[i] == "ALIVE")
      part.output.push_back(nbits+i-1);
    else
      cout<<"error: "<<variables[i]<<endl;
  }

  return part;
}

BDD DFALoader::state2bdd(size_t s,
                         size_t nbits,
                         const vector<BDD>& bddvars) const {
  string bin = state2bin(s);
  BDD b = mgr->bddOne();
  int nzero = nbits - bin.length();
  //cout<<nzero<<endl;
  for(int i = 0; i < nzero; i++){
    b *= !bddvars[i];
  }
  for(int i = 0; i < bin.length(); i++){
    if(bin[i] == '0')
      b *= !bddvars[i+nzero];
    else
      b *= bddvars[i+nzero];
  }
  return b;

}
