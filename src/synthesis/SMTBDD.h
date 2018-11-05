#ifndef SMTBDD_H
#define SMTBDD_H

class SMTBDDNode
{
 public:

  SMTBDDNode(size_t f, size_t s, size_t t);
  
  bool is_terminal() const;
  size_t id() const;
  size_t left_branch() const;
  size_t right_branch() const;
  size_t terminal_value() const;

 private:

  size_t first, second, third;
};

class SMTBDD
{
 public:

  SMTBDD(std::vector<size_t> behavior,
	 std::vector<SMTBDDNode> nodes);

  std::vector<ADD> interpret(const VarPartition& partition,
			     const BDDDict& bdd_dict);
};

#endif // SMTBDD_H
