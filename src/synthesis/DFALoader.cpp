#include "DFALoader.h"

#include <fstream>
#include <iostream>
#include <set>

#include <boost/algorithm/string.hpp>

using std::cout;
using std::endl;
using std::ifstream;
using boost::algorithm::is_any_of;
using boost::algorithm::split;
using std::string;
using std::vector;

DFA DFALoader::run(const string& filename, const string& partfile)
{
  DFA dfa = read_from_file(filename);
  dfa.partition = read_partfile(partfile);

  return dfa;
}

DFA DFALoader::read_from_file(const string& filename) const {
  ifstream f(filename);
  DFA dfa;
  
  if(f.is_open()){
    bool flag = 0;
    string line;
    vector<size_t> tmp;
    vector<string> fields; //temporary varibale

    while(getline(f, line)){
      if(flag == 0){
        if(strfind(line, "number of variables")){
          split(fields, line, is_any_of(" "));
          dfa.number_of_vars = stoi(fields[3]);
          //cout<<nvars<<endl;
        }
        if(strfind(line, "variables") && !strfind(line, "number")){
          split(dfa.var_names, line, is_any_of(" "));

        }
        else if(strfind(line, "states")){
          split(fields, line, is_any_of(" "));
          dfa.number_of_states = stoi(fields[1]);
          // cout<<nstates<<endl;
        }
        else if(strfind(line, "initial")){
          split(fields, line, is_any_of(" "));
          dfa.initial_state = stoi(fields[1]);
          //cout<<init<<endl;
        }
        else if(strfind(line, "bdd nodes")){
          split(fields, line, is_any_of(" "));
          dfa.number_of_nodes = stoi(fields[2]);
          //cout<<nodes<<endl;
        }
        else if(strfind(line, "final")){
          split(fields, line, is_any_of(" "));
          int i = 1; // start at 1 to ignore "final" token
          while(i < fields.size()){
            if(fields[i] == "1")
              dfa.final_states.push_back(i-1);
            i = i + 1;
          }
          //print_int(finalstates);
        }
        else if(strfind(line, "behaviour")){
          split(fields, line, is_any_of(" "));
          int i = 1;
          while(i < fields.size()){
            dfa.behaviour.push_back(stoi(fields[i]));
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
        dfa.smtbdd.push_back(tmp);
        tmp.clear();
      }
    }

  }
  f.close();

  return dfa;
}

bool DFALoader::strfind(const string& str, const string& target) const {
  size_t found = str.find(target);
  if(found != string::npos)
    return true;
  else
    return false;
}

IOPartition DFALoader::read_partfile(const string& partfile) const {
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

  IOPartition part;
  for(int i = 1; i < inputs.size(); i++)
    part.input_vars.insert(inputs[i]);
  for(int i = 1; i < outputs.size(); i++)
    part.output_vars.insert(outputs[i]);

  return part;
}
