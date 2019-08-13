#pragma once

#include <unordered_map>
#include <vector>

#include "AttrRanking.hpp"
#include "JoinAlgorithm.hpp"

#include "SyftMgr.h"
#include "SymbolicDFA.h"

class JoinAlgFactory
{
  std::unordered_map<std::string, std::string> _options;

  jet::AttrRanking compute_mcs(const std::vector<SymbolicDFA>& dfas,
                               const jet::AttrSet& vertices,
                               const SyftMgr& mgr);

  jet::AttrRanking compute_ranking(const std::vector<SymbolicDFA>& dfas,
                                   const jet::AttrSet& relevant_vars,
                                   const SyftMgr& mgr);

public:

  JoinAlgFactory(std::unordered_map<std::string, std::string> options);

  std::unique_ptr<jet::JoinAlgorithm> build(
    const std::vector<SymbolicDFA>& dfas,
    const SyftMgr& mgr);
};
