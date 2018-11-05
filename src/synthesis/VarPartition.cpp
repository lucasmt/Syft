#include "VarPartition.h"

using std::string;
using std::vector;

VarPartition::VarPartition(const vector<string>& env_names,
			   const vector<string>& sys_names)
{
  vector<jet::Attr> env_vars, sys_vars;
  size_t id = 0;

  for (string name : env_vars)
  {
    jet::Attr var(id);
    ++id;

    env.push_back(var);
    _name_to_var[name] = var;
    _var_to_name[var] = name;
  }

  for (string name : sys_vars)
  {
    jet::Attr var(id);
    ++id;

    sys.push_back(var);
    _name_to_var[name] = var;
    _var_to_name[var] = name;
  }

  _env_vars = jet::AttrSet(env);
  _sys_vars = jet::AttrSet(sys);
}

VarPartition VarPartition::load(const string& partition_file) const {
  ifstream f(partfile);

  if (!f.is_open())
    throw runtime_error("File could not be opened: " + partition_file);
  
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
