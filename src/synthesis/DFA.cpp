#include "DFA.h"

#include <fstream>

#include <boost/algorithm/string.hpp>

using boost::algorithm::is_any_of;
using boost::algorithm::split;
using std::ifstream;
using std::move;
using std::runtime_error;
using std::string;
using std::to_string;
using std::vector;

DFAState::DFAState(size_t dfa_index, size_t id)
  : _dfa_index(dfa_index)
  , _id(id)
{}

size_t DFAState::dfa_index() const { return _dfa_index; }
size_t DFAState::id() const { return _id; }

DFA::DFA(size_t index,
         size_t number_of_states,
         jet::AttrSet env_vars,
         jet::AttrSet sys_vars,
         size_t initial_state,
         SMTBDD transition_function,
         std::vector<size_t> accepting_states)
  : _index(index)
  , _number_of_states(number_of_states)
  , _env_vars(env_vars)
  , _sys_vars(sys_vars)
  , _initial_state(index, initial_state)
  , _transition_function(transition_function)
{
  for (size_t id : accepting_states)
  {
    _accepting_states.emplace_back(index, id);
  }
}

size_t DFA::index() const { return _index; }
size_t DFA::number_of_states() const { return _number_of_states; }
jet::AttrSet DFA::env_vars() const { return _env_vars; }
jet::AttrSet DFA::sys_vars() const { return _sys_vars; }
DFAState DFA::initial_state() const { return _initial_state; }
const SMTBDD& DFA::transition_function() const { return _transition_function; }
const vector<DFAState>& DFA::accepting_states() const
{
  return _accepting_states;
}

DFAState DFA::ith_state(size_t i) const
{
  return DFAState(_index, i);
}

bool strfind(const string& str, const string& target) {
  size_t found = str.find(target);
  if(found != string::npos)
    return true;
  else
    return false;
}

DFA DFA::load_from_file(const string& base_filename,
                        size_t index,
                        const VarPartition& var_partition)
{
  string full_filename = base_filename + "_" + to_string(index) + ".dfa";
  ifstream f(full_filename);

  if (!f.is_open())
    throw runtime_error("Unable to open file: " + full_filename);

  string line;
  vector<string> fields; // temporary varibale

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
      var_names.erase(var_names.begin()); // remove "variables" token
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
      int i = 1; // skip "final" token
      while(i < fields.size()){
	if(fields[i] == "1")
	  accepting_states.push_back(i-1);
	++i;
      }
    }
    else if(strfind(line, "behaviour")){
      split(fields, line, is_any_of(" "));
      int i = 1; // skip "behaviour" token
      while(i < fields.size()){
	behavior.push_back(stoi(fields[i]));
	++i;
      }
    }
    else if(strfind(line, "bdd:")) {

      while (getline(f, line) && !strfind(line, "end")) {
	split(fields, line, is_any_of(" "));
	
        int first = stoi(fields[1]); // skip "" token

        SMTBDDNode node = (first == -1)
          ? SMTBDDNode::terminal(stoi(fields[2]))
          : SMTBDDNode::ite(var_partition.from_name(var_names[first]),
                            stoi(fields[2]),
                            stoi(fields[3]));
          
	nodes.push_back(node);
      }
    }
  }

  f.close();
  
  SMTBDD smtbdd(behavior, nodes);

  // need to remove extraneous initial state that MONA produces for some reason

  // extraneous initial state unconditionally transitions to true initial state
  SMTBDDNode initial_node = smtbdd.at(initial_state);  
  assert(initial_node.is_terminal());
  size_t true_initial_state = initial_node.terminal_value();
  
  return DFA(index,
	     number_of_states,
             var_partition.env_vars(var_names),
             var_partition.sys_vars(var_names),
	     true_initial_state,
	     move(smtbdd),
	     move(accepting_states));
}


