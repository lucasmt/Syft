#pragma once

#include "SyftMgr.h"
#include "SymbolicDFA.h"

class Instance
{
public:

  SyftMgr mgr;
  std::vector<SymbolicDFA> dfas;
};

class InstanceFactory
{
public:

  Instance build(const std::string& instance_name) const;
};
