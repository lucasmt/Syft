#include "DFAGameSolver.h"

#include <memory>

using std::string;
using std::unordered_map;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;
using std::move;

DFAGameSolver::DFAGameSolver(shared_ptr<Cudd> m, string filename, string partfile)
{
    //ctor

    //Cudd *p = &mgr;
    bdd = make_unique<DFA>(m);
    bdd->initialize(filename, partfile);
    mgr = m;
    initializer();

    bdd->bdd2dot();

}

DFAGameSolver::DFAGameSolver(shared_ptr<Cudd> m, unique_ptr<DFA> d)
{
    bdd = move(d);
    mgr = move(m);
    initializer();

    bdd->bdd2dot();
}

DFAGameSolver::~DFAGameSolver()
{
    //dtor
}

void DFAGameSolver::initializer(){
  for(int i = 0; i < bdd->nbits; i++){
    BDD b = mgr->bddVar();
    bdd->bddvars.push_back(b);
  }
    W.push_back(bdd->finalstatesBDD);
    Wprime.push_back(bdd->finalstatesBDD);
    cur = 0;


}

BDD DFAGameSolver::state2bdd(int s){
    string bin = state2bin(s);
    BDD b = mgr->bddOne();
    int nzero = bdd->nbits - bin.length();
    //cout<<nzero<<endl;
    for(int i = 0; i < nzero; i++){
        b *= !bdd->bddvars[i];
    }
    for(int i = 0; i < bin.length(); i++){
        if(bin[i] == '0')
            b *= !bdd->bddvars[i+nzero];
        else
            b *= bdd->bddvars[i+nzero];
    }
    return b;

}

string DFAGameSolver::state2bin(int n){
    string res;
    while (n)
    {
        res.push_back((n & 1) + '0');
        n >>= 1;
    }

    if (res.empty())
        res = "0";
    else
        reverse(res.begin(), res.end());
   return res;
}

bool DFAGameSolver::fixpoint(){
    if(W[cur] == W[cur-1])
        return true;
}

void DFAGameSolver::printBDDSat(BDD b){

  std::cout<<"sat with: ";
  int max = bdd->nstates;
  
  for (int i=0; i<max; i++){
    if (b.Eval(state2bit(i)).IsOne()){
      std::cout<<i<<", ";
    }
  }
  std::cout<<std::endl;
}

my::optional<unordered_map<unsigned int, BDD>> DFAGameSolver::realizablity(){
    while(true){
        //cout<<"interative"<<endl;
        //dumpdot(W[cur], "W"+to_string(cur));
        //dumpdot(Wprime[cur], "Wprme"+to_string(cur));
        BDD tmp = W[cur] + univsyn();
        W.push_back(tmp);
        cur++;
        //dumpdot(W[cur], "W"+to_string(cur));
	Wprime.push_back(existsyn());
        if(fixpoint())
            break;
	//        Wprime.push_back(existsyn());
        //assert(cur = (W.size() - 1));
    }
    if(Wprime[cur-1].Eval(bdd->initbv.data()).IsOne()){
        BDD O = mgr->bddOne();
	//        vector<BDD> S2O;
        for(int i = 0; i < bdd->output.size(); i++){
            //cout<<bdd->output[i]<<endl;
            O *= bdd->bddvars[bdd->output[i]];
        }
	/*
        O *= bdd->bddvars[bdd->nbits];
        //naive synthesis
        BDD cons = W[cur].SolveEqn(O, S2O, &yindex, bdd->output.size());
        strategy(S2O);
	*/
	
        InputFirstSynthesis IFsyn(*mgr);

	return IFsyn.synthesize(W[cur], O);
    }
    std::cout<<"unrealizable, winning set: "<<std::endl;
    std::cout<<Wprime[Wprime.size()-1]<<std::endl;
    assert(false);
    return my::nullopt;
}

my::optional<unordered_map<unsigned int, BDD>> DFAGameSolver::realizablity_variant(){
    BDD transducer;
    while(true){
        int index;
        BDD O = mgr->bddOne();
        for(int i = 0; i < bdd->output.size(); i++){
            index = bdd->output[i];
            O *= bdd->bddvars[index];
        }

        BDD tmp = W[cur] + existsyn_invariant(O, transducer);
        W.push_back(tmp);
        cur++;
        if(fixpoint())
            break;

        BDD I = mgr->bddOne();
        for(int i = 0; i < bdd->input.size(); i++){
            index = bdd->input[i];
            I *= bdd->bddvars[index];
        }

        Wprime.push_back(univsyn_invariant(I));
        if((Wprime[cur].Eval(bdd->initbv.data())).IsOne()){
            return unordered_map<unsigned int, BDD>();
        }

    }
    if((Wprime[cur-1].Eval(bdd->initbv.data())).IsOne()){
      // TODO: use ifstrategysynthesis
        BDD O = mgr->bddOne();
	vector<BDD> S2O;
        for(int i = 0; i < bdd->output.size(); i++){
            O *= bdd->bddvars[bdd->output[i]];
        }
        O *= bdd->bddvars[bdd->nbits];
        //naive synthesis
        transducer.SolveEqn(O, S2O, outindex(), bdd->output.size());
        strategy(S2O);

        return unordered_map<unsigned int, BDD>();
    }
    return my::nullopt;

}


void DFAGameSolver::strategy(vector<BDD>& S2O){
    vector<BDD> winning;
    for(int i = 0; i < S2O.size(); i++){
        //dumpdot(S2O[i], "S2O"+to_string(i));
        for(int j = 0; j < bdd->output.size(); j++){
            int index = bdd->output[j];
            S2O[i] = S2O[i].Compose(bdd->bddvars[index], mgr->bddOne());
        }
    }
}

int** DFAGameSolver::outindex(){
    int outlength = bdd->output.size();
    int outwidth = 2;
    int **out = 0;
    out = new int*[outlength];
    for(int l = 0; l < outlength; l++){
        out[l] = new int[outwidth];
        out[l][0] = l;
        out[l][1] = bdd->output[l];
    }
    return out;
}

int* DFAGameSolver::state2bit(int n){
    int* s = new int[bdd->nbits];
    for (int i=bdd->nbits-1; i>=0; i--){
      s[i] = n%2;
      n = n/2;
    }
    return s;
}


BDD DFAGameSolver::univsyn(){
    BDD I = mgr->bddOne();
    BDD tmp = Wprime[cur];
    int index;
    int offset = bdd->nbits + bdd->nvars;
    for(int i = 0; i < bdd->input.size(); i++){
        index = bdd->input[i];
        I *= bdd->bddvars[index];
    }
    //dumpdot(I, "W00");
    tmp = prime(tmp);
    //dumpdot(tmp, "s-s'"+to_string(cur));
    for(int i = 0; i < bdd->nbits; i++){
        tmp = tmp.Compose(bdd->res[i], offset+i);
        //dumpdot(tmp, "s.compose'"+to_string(i));
    }
    //dumpdot(tmp, "W00");

    tmp *= !Wprime[cur];

    BDD eliminput = tmp.UnivAbstract(I);
    //dumpdot(eliminput, "W01");
    //dumpdot(eliminput, "EU"+to_string(cur));
    return eliminput;

}

BDD DFAGameSolver::existsyn_invariant(BDD exist, BDD& transducer){
    BDD tmp = Wprime[cur];
    int offset = bdd->nbits + bdd->nvars;

    //dumpdot(I, "W00");
    tmp = prime(tmp);
    for(int i = 0; i < bdd->nbits; i++){
        tmp = tmp.Compose(bdd->res[i], offset+i);
    }
    transducer = tmp;
    tmp *= !Wprime[cur];
    BDD elimoutput = tmp.ExistAbstract(exist);
    return elimoutput;

}

BDD DFAGameSolver::univsyn_invariant(BDD univ){

    BDD tmp = W[cur];
    BDD elimuniv = tmp.UnivAbstract(univ);
    return elimuniv;

}

BDD DFAGameSolver::prime(BDD orign){
    int offset = bdd->nbits + bdd->nvars;
    BDD tmp = orign;
    for(int i = 0; i < bdd->nbits; i++){
        tmp = tmp.Compose(bdd->bddvars[i+offset], i);
    }
    return tmp;
}

BDD DFAGameSolver::existsyn(){
    BDD O = mgr->bddOne();
    BDD tmp = W[cur];
    int index;
    int offset = bdd->nbits + bdd->nvars;
    for(int i = 0; i < bdd->output.size(); i++){
        index = bdd->output[i];
        O *= bdd->bddvars[index];
    }
    BDD elimoutput = tmp.ExistAbstract(O);
    return elimoutput;

}

void DFAGameSolver::dumpdot(BDD &b, string filename){
    FILE *fp = fopen(filename.c_str(), "w");
    vector<BDD> single(1);
    single[0] = b;
	this->mgr->DumpDot(single, NULL, NULL, fp);
	fclose(fp);
}


















