#include "DFALoader.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

using boost::algorithm::is_any_of;
using boost::algorithm::split;
using std::cout;
using std::endl;
using std::ifstream;
using std::move;
using std::runtime_error;
using std::string;
using std::to_string;
using std::vector;

DFA DFALoader::run(const string& base_filename,
                   size_t index,
                   const VarPartition& partition) const
{
  string full_filename = base_filename + to_string(index) + ".dfa";
  ifstream f(full_filename);

  if (!f.is_open())
    throw runtime_error("Unable to open file: " + full_filename);

  string line;
  vector<string> fields; //temporary varibale

  vector<string> var_names;
  size_t number_of_states;
  size_t initial_state;
  vector<size_t> accepting_states;
  vector<size_t> behavior;
  vector<SMTBDDNode> nodes;
  
  while(getline(f, line)){
    if(strfind(line, "number of variables")){
      split(fields, line, is_any_of(" "));
      size_t number_of_vars = stoi(fields[3]);
    }
    else if(strfind(line, "variables")){
      split(var_names, line, is_any_of(" "));
    }
    else if(strfind(line, "states")){
      split(fields, line, is_any_of(" "));
      number_of_states = stoi(fields[1]);
    }
    else if(strfind(line, "initial")){
      split(fields, line, is_any_of(" "));
      initial_state = stoi(fields[1]);
    }
    else if(strfind(line, "bdd nodes")){
      split(fields, line, is_any_of(" "));
      nodes.reserve(stoi(fields[2]));
    }
    else if(strfind(line, "final")){
      split(fields, line, is_any_of(" "));
      int i = 1; // start at 1 to ignore "final" token
      while(i < fields.size()){
	if(fields[i] == "1")
	  accepting_states.push_back(i-1);
	++i;
      }
    }
    else if(strfind(line, "behaviour")){
      split(fields, line, is_any_of(" "));
      int i = 1;
      while(i < fields.size()){
	behavior.push_back(stoi(fields[i]));
	++i;
      }
    }
    else if(strfind(line, "bdd:")) {

      while (!strfind(line, "end")) {
	split(fields, line, is_any_of(" "));

	// starts at 1 because of leading whitespace?
        int first = stoi(fields[1]);

        SMTBDDNode node = (first == -1)
          ? SMTBDDNode::terminal(stoi(fields[2]))
          : SMTBDDNode::ite(partition.from_name(var_names[first]),
                            stoi(fields[2]),
                            stoi(fields[3]));
          
	nodes.push_back(node);
      }
    }
  }

  f.close();
  
  SMTBDD smtbdd(behavior, nodes);
  
  return DFA(index,
	     number_of_states,
             partition.env_vars(var_names),
             partition.sys_vars(var_names),
	     initial_state,
	     move(smtbdd),
	     move(accepting_states));
}

bool DFALoader::strfind(const string& str, const string& target) const {
  size_t found = str.find(target);
  if(found != string::npos)
    return true;
  else
    return false;
}
