#ifndef SYMBOLICDFACONVERTER_H
#define SYMBOLICDFACONVERTER_H

#include <memory>
#include <vector>

#include "cuddObj.hh"

#include "BDDMgr.hpp"

#include "DFA.h"
#include "SymbolicDFA.h"

class SymbolicDFAConverter
{
public:

  SymbolicDFAConverter(const StateMap& state_map, const BDDDict& bdd_dict);
  
  SymbolicDFA run(const DFA& dfa);

private:

  const StateMap& _state_map;
  const BDDDict& _bdd_dict;
  //DFAArgs read_from_file(const std::string& filename) const;
  //bool strfind(const std::string& str, const std::string& target) const;
  std::string state2bin(int n) const;
  void construct_bdd_new(SymbolicDFA& symbolic_dfa, const DFA& dfa) const;

  
  std::vector<BDD> try_get(
    size_t index,
    std::vector<std::vector<BDD>>& tBDD,
    const std::vector<std::vector<size_t>>& smtbdd,
    size_t nbits,
    const std::vector<BDD>& bddvars) const;

  BDD var2bddvar(int v, int index, const std::vector<BDD>& bddvars) const;

  /*IOPartition read_partfile(const std::string& partfile,
                            const std::vector<std::string>& variables,
                            size_t nbits) const;*/

  BDD state2bdd(size_t s, size_t nbits, const std::vector<BDD>& bddvars) const;
};

#endif // SYMBOLICDFACONVERTER_H
