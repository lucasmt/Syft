#include "LTLInterp.h"

#include "Attr.hpp"

using std::logic_error;
using std::vector;

LTLInterp::LTLInterp(vector<ltl_formula*> formulas)
  : _formulas(move(formulas))
{}

ltl_formula* LTLInterp::emptyInterp()
{
  return create_operation(eTRUE, NULL, NULL);
}

ltl_formula* LTLInterp::leafInterp(size_t id)
{
  return _formulas[id];
}

ltl_formula* LTLInterp::joinInterp(const std::vector<ltl_formula*>& args)
{
  if (args.size() == 0) return emptyInterp();

  ltl_formula* conjunction = args[0];

  for (size_t i = 1; i < args.size(); ++i)
  {
    conjunction = create_operation(eAND, conjunction, args[i]);
  }
  
  return conjunction;
}

ltl_formula* LTLInterp::projInterp(const jet::AttrSet& projAttrs,
				   ltl_formula* const& arg)
{
  throw logic_error("No interpretation for projection in LTLInterp");
}
