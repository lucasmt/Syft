#ifndef DFALOADER_H
#define DFALOADER_H

#include <memory>
#include <vector>

#include "cuddObj.hh"

#include "DFA.h"

class DFALoader
{
public:

  DFALoader(std::shared_ptr<Cudd> m);
  
  DFA run(const std::string& filename, const std::string& partfile);

private:

  std::shared_ptr<Cudd> mgr;
  DFAArgs read_from_file(const std::string& filename) const;
  bool strfind(const std::string& str, const std::string& target) const;
  std::string state2bin(int n) const;
  DFABDDs construct_bdd_new(size_t nbits, const DFAArgs& args) const;

  
  std::vector<BDD> try_get(
    size_t index,
    std::vector<std::vector<BDD>>& tBDD,
    const std::vector<std::vector<size_t>>& smtbdd,
    size_t nbits,
    const std::vector<BDD>& bddvars) const;

  BDD var2bddvar(int v, int index, const std::vector<BDD>& bddvars) const;

  IOPartition read_partfile(const std::string& partfile,
                            const std::vector<std::string>& variables,
                            size_t nbits) const;

  BDD state2bdd(size_t s, size_t nbits, const std::vector<BDD>& bddvars) const;
};

#endif // DFALOADER_H
