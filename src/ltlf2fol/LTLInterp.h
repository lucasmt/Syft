#pragma once

#include "Attr.hpp"
#include "JETInterp.hpp"

#include "ltl_formula.h"

class LTLInterp : public jet::JETInterp<ltl_formula*>
{
  std::vector<ltl_formula*> _formulas;
  
public:

  LTLInterp(std::vector<ltl_formula*> formulas);
  
  ltl_formula* emptyInterp();

  ltl_formula* leafInterp(size_t id);

  ltl_formula* joinInterp(const std::vector<ltl_formula*>& args);

  ltl_formula* projInterp(const jet::AttrSet& projAttrs,
			  ltl_formula* const& arg);
};
