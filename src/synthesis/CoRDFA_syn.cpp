#include "CoRDFA_syn.h"

using std::make_unique;
using std::move;
using std::set;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

CoRDFA_syn::CoRDFA_syn(shared_ptr<Cudd> m, string filename, string partfile)
{
    unique_ptr<SSNFA> ssnfa = make_unique<SSNFA>(m);
    ssnfa->initialize_mona(filename, partfile);

    mgr = move(m);
    ssnfa->project_unobservables();
    ssnfa->complement();
    initializer(ssnfa);
    bdd = move(ssnfa);
}

CoRDFA_syn::~CoRDFA_syn() {}

void CoRDFA_syn::initializer(unique_ptr<SSNFA>& ssnfa){
  for(int i = 0; i < ssnfa->nbits; i++){
    BDD b = mgr->bddVar();
    ssnfa->bddvars.push_back(b);
  }
  BDD t = ssnfa->finalstatesBDD;
  W.push_back(ssnfa->finalstatesBDD);
  Wprime.push_back(ssnfa->finalstatesBDD);
  cur = 0;

  for (int i = 0; i < ssnfa->nstates; ++i) {
    ssnfa->res.push_back(mgr->bddZero());
    for (int j = 0; j < ssnfa->nstates; ++j) {
      BDD trans = ssnfa->bddvars[j] & ssnfa->labels[i][j];
      ssnfa->res[i] |= trans;
    }
  }
}
















