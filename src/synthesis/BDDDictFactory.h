#include <memory>
#include <vector>

#include "cuddObj.hh"

#include "BDDDict.hpp"

#include "StateMap.h"
#include "VarPartition.h"

class BDDDictFactory
{
  std::string _option;

public:

  BDDDictFactory(std::string option);

  std::shared_ptr<BDDDict> build(const std::vector<DFA>& dfas,
                                 const Cudd& cudd_mgr,
                                 const VarPartition& var_partition,
                                 const StateMap& state_map) const;
};
