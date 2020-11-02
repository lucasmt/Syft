#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include "syn.h"
#include "CoRDFA_syn.h"

using std::string;
using std::shared_ptr;
using std::make_shared;
using std::move;
using std::cout;
using std::endl;
namespace chrono = std::chrono;
using std::unique_ptr;
using std::make_unique;

string get_DFAfile(string LTLFfile){
    string FOL = LTLFfile+".mona";
    string LTLF2FOL = "./ltlf2fol NNF "+LTLFfile+" >"+FOL;
    system(LTLF2FOL.c_str());

    string DFA = LTLFfile+".dfa";
    string FOL2DFA = "mona -u -xw "+FOL+" >"+DFA;
    system(FOL2DFA.c_str());

    return DFA;
}

int main(int argc, char ** argv){
    clock_t c_start = clock();
    auto t_start = chrono::high_resolution_clock::now();
    string filename;
    string partfile;
    string autfile;
    string starting_player;
    string observability;
    string spec_type;
    if(argc != 5){
        cout<<"Usage: ./Syft DFAfile Partfile Starting_player(0: system, 1: environment) Observability(partial, full) SpecType(dfa, cordfa)"<<endl;
        return 0;
    }
    else{
        filename = argv[1];
        partfile = argv[2];
        starting_player = argv[3];
	observability = argv[4];
	spec_type = argv[5];
    }

    bool partial_observability;

    if (observability == "partial")
      partial_observability = true;
    else if (observability == "full")
      partial_observability = false;
    else {
      cout << "Observability should be one of: partial, full" << endl;
      return 0;
    }

    bool cordfa_spec;

    if (spec_type == "dfa")
      cordfa_spec = false;
    else if (spec_type == "cordfa")
      cordfa_spec = true;
    else {
      cout << "SpecType should be one of: dfa, cordfa" << endl;
      return 0;
    }

    shared_ptr<Cudd> mgr = make_shared<Cudd>();
    clock_t c_mona_dfa_end = clock();
    auto t_mona_dfa_end = chrono::high_resolution_clock::now();
    autfile = get_DFAfile(filename);
    std::cout << "DFA constructed by MONA CPU time used: "
              << 1000.0 * (c_mona_dfa_end-c_start) / CLOCKS_PER_SEC << " ms\n"
              << "DFA constructed by MONA wall clock time passed: "
              << std::chrono::duration<double, std::milli>(t_mona_dfa_end-t_start).count()
              << " ms\n";

    unique_ptr<syn> test =
      cordfa_spec
      ? make_unique<CoRDFA_syn>(move(mgr), autfile, partfile)
      : make_unique<syn>(move(mgr), autfile, partfile, partial_observability);
    
    clock_t c_dfa_end = clock();
    auto t_dfa_end = chrono::high_resolution_clock::now();
    std::cout << "DFA CPU time used: "
              << 1000.0 * (c_dfa_end-c_start) / CLOCKS_PER_SEC << " ms\n"
              << "DFA wall clock time passed: "
              << std::chrono::duration<double, std::milli>(t_dfa_end-t_start).count()
              << " ms\n";

    bool res = 0;
    std::unordered_map<unsigned, BDD> strategy;

    if(starting_player == "1")
        res = test->realizablity_env(strategy);
    else
        res = test->realizablity_sys(strategy);

    if(res)
        cout<<"realizable"<<endl;
    else
        cout<<"unrealizable"<<endl;
    clock_t c_end = clock();
    auto t_end = chrono::high_resolution_clock::now();
    std::cout << "Total CPU time used: "
              << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms\n"
              << "Total wall clock time passed: "
              << std::chrono::duration<double, std::milli>(t_end-t_start).count()
              << " ms\n";
    return 0;

}
//solveeqn
