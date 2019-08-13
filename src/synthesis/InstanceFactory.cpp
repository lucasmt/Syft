#include "InstanceFactory.h"

Instance InstanceFactory::build_counters(size_t n)
{
  VarPartition var_partition = counters_partition(n);
  StateMap state_map = counters_state_map(n);
  
  vector<BDD> transitions;
  vector<BDD> acceptance;

  
}

Instance InstanceFactory::build(const std::string& instance_name, size_t n)
{
  if (instance_name == "counters")
  {
    return build_counters(n);
  }
  else
  {
    throw runtime_error("Invalid instance name: " + instance_name);
  }
}
