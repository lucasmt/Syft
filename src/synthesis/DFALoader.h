#ifndef DFALOADER_H
#define DFALOADER_H

#include <memory>

#include "DFA.h"

class DFALoader
{
public:

  DFALoader(std::shared_ptr<Cudd> m);
  
  DFA run(std::string filename, std::string partfile);

private:

  std::shared_ptr<Cudd> mgr;
};

#endif // DFALOADER_H
