#ifndef VARPARTITION_H
#define VARPARTITION_H

#include <unordered_map>
#include <vector>

#include "Attr.hpp"

class VarPartition
{
  jet::AttrSet _env_vars;
  jet::AttrSet _sys_vars;

  std::unordered_map<std::string, jet::Attr> _name_to_var;
  std::unordered_map<jet::Attr, std::string> _var_to_name;
  
public:

  VarPartition(const std::vector<std::string>& env_vars,
	       const std::vector<std::string>& sys_vars);
  
  static VarPartition load(const std::string& partition_file);

  jet::AttrSet env_vars() const;
  jet::AttrSet sys_vars() const;
  jet::AttrSet env_vars(std::vector<std::string> names) const;
  jet::AttrSet sys_vars(std::vector<std::string> names) const;

  jet::Attr from_name(std::string name) const;

  size_t max_id() const;
};

#endif // VARPARTITION_H
