#include "VarPartition.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

using boost::algorithm::is_any_of;
using boost::algorithm::split;
using std::ifstream;
using std::max;
using std::runtime_error;
using std::string;
using std::vector;

VarPartition::VarPartition(const vector<string>& env_names,
			   const vector<string>& sys_names)
{
  vector<jet::Attr> env_vars, sys_vars;
  size_t id = 0;

  for (string name : env_names)
  {
    jet::Attr var(id);
    ++id;

    env_vars.push_back(var);
    _name_to_var.insert({ name, var });
    _var_to_name.insert({ var, name });
  }

  for (string name : sys_names)
  {
    jet::Attr var(id);
    ++id;

    sys_vars.push_back(var);
    _name_to_var.insert({ name, var });
    _var_to_name.insert({ var, name });
  }

  _env_vars = jet::AttrSet(env_vars);
  _sys_vars = jet::AttrSet(sys_vars);
}

VarPartition VarPartition::load(const string& partition_file) {
  ifstream f(partition_file);

  if (!f.is_open())
    throw runtime_error("File could not be opened: " + partition_file);
  
  vector<string> inputs;
  vector<string> outputs;
  string line;
  while(getline(f, line)){
    if(f.is_open()){
      if(line.compare(0, 6, "inputs") == 0){
	split(inputs, line, is_any_of(" "));
      }
      else if(line.compare(0, 7, "outputs") == 0){
	split(outputs, line, is_any_of(" "));
      }
      else{
	throw runtime_error("Invalid partition file format: "
			    + partition_file);
	//cout<<"read partfile error!"<<endl;
	//cout<<partfile<<endl;
	//cout<<line<<endl;
      }
    }
  }
  f.close();

  inputs.erase(inputs.begin());
  outputs.erase(outputs.begin());

  VarPartition partition(inputs, outputs);

  return partition;
}

size_t VarPartition::max_id() const
{
  size_t max_env = _env_vars.maxElem()->id();
  size_t max_sys = _sys_vars.maxElem()->id();
  
  return max(max_env, max_sys);
}