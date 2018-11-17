#ifndef SMTBDD_H
#define SMTBDD_H

#include <vector>

#include "cuddObj.hh"

#include "Attr.hpp"

class SMTBDDNode
{
 public:

  static SMTBDDNode ite(jet::Attr var, size_t left_index, size_t right_index);
  static SMTBDDNode terminal(size_t value); 
  
  bool is_terminal() const;
  jet::Attr var() const;
  size_t left_index() const;
  size_t right_index() const;
  size_t terminal_value() const;
  
 private:

  SMTBDDNode(size_t id, size_t left_index, size_t right_index);
  SMTBDDNode(size_t value);
  
  bool _is_terminal;
  size_t _value_or_id;
  size_t _left_index;
  size_t _right_index;
};

class SMTBDD
{
public:

  SMTBDD(std::vector<size_t> behavior,
	 std::vector<SMTBDDNode> nodes);

  const std::vector<size_t>& behavior() const;
  const std::vector<SMTBDDNode>& nodes() const;

  SMTBDDNode at(size_t state) const;
  
private:

  std::vector<size_t> _behavior;
  std::vector<SMTBDDNode> _nodes;
};

#endif // SMTBDD_H
