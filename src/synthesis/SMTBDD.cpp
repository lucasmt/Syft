#include "SMTBDD.h"

using std::vector;

SMTBDDNode::SMTBDDNode(size_t value)
{
  _is_terminal = true;
  _value_or_id = value;
}

SMTBDDNode::SMTBDDNode(size_t id, size_t left_index, size_t right_index)
{
  _is_terminal = false;
  _value_or_id = id;
  _left_index = left_index;
  _right_index = right_index;
}

SMTBDDNode SMTBDDNode::ite(jet::Attr var, size_t left_index, size_t right_index)
{
  return SMTBDDNode(var.id(), left_index, right_index);
}

SMTBDDNode SMTBDDNode::terminal(size_t value)
{
  return SMTBDDNode(value);
}

bool SMTBDDNode::is_terminal() const { return _is_terminal; }
jet::Attr SMTBDDNode::var() const { return jet::Attr(_value_or_id); }
size_t SMTBDDNode::left_index() const { return _left_index; }
size_t SMTBDDNode::right_index() const { return _right_index; }
size_t SMTBDDNode::terminal_value() const { return _value_or_id; }

SMTBDD::SMTBDD(vector<size_t> behavior, vector<SMTBDDNode> nodes)
  : _behavior(move(behavior))
  , _nodes(move(nodes))
{}

const vector<size_t>& SMTBDD::behavior() const
{
  return _behavior;
}

const vector<SMTBDDNode>& SMTBDD::nodes() const
{
  return _nodes;
}
 
SMTBDDNode SMTBDD::at(size_t state) const
{
  return _nodes[_behavior[state]];
}
