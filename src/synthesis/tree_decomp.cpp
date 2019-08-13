#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "cuddObj.hh"

#include "DFALoader.h"
#include "SyftMgr.h"
#include "SymbolicDFA.h"
#include "SymbolicDFAConverter.h"

using std::cout;
using std::endl;
using std::make_pair;
using std::move;
using std::ofstream;
using std::pair;
using std::string;
using std::unordered_map;
using std::vector;

int main(int argc, char ** argv){

  if(argc != 5){
    cout << "Usage: " << argv[0]
         << " <base-filename> <number-of-files> <partition-file> <out-file>"
         << endl;
      return 0;
    }

    string base_filename = argv[1];
    size_t k = atoi(argv[2]);
    string partition_file = argv[3];
    string output_file = argv[4];

    VarPartition var_partition = VarPartition::load(partition_file);

    vector<DFA> dfas;
    dfas.reserve(k);
    
    for (size_t i = 0; i < k; i++)
    {
      DFA dfa = DFA::load_from_file(base_filename, i, var_partition);
      dfas.push_back(dfa);
    }
    
    SyftMgr mgr(dfas, move(var_partition));
    
    vector<SymbolicDFA> symbolic_dfas;
    symbolic_dfas.reserve(k);

    for (size_t i = 0; i < k; i++)
    {
      symbolic_dfas.push_back(SymbolicDFA(dfas[i], mgr));
    }

    vector<jet::AttrSet> hyperedges;
    hyperedges.reserve(symbolic_dfas.size() + 1);

    for (const SymbolicDFA& dfa : symbolic_dfas)
    {
      hyperedges.push_back(dfa.output_vars());
    }

    hyperedges.push_back(mgr.state_map.next_state_vars());

    unordered_map<jet::Attr, size_t> vertices;
    size_t vertex_id = 0;

    for (jet::Attr var : mgr.output_vars())
    {
      ++vertex_id;
      vertices[var] = vertex_id;
    }

    vector<pair<size_t, size_t>> edges;

    for (const jet::AttrSet& hyperedge : hyperedges)
    {
      for (auto it1 = hyperedge.begin(); it1 != hyperedge.end(); ++it1)
      {
        for (auto it2 = it1 + 1; it2 != hyperedge.end(); ++it2)
        {
          edges.push_back(make_pair(vertices[*it1], vertices[*it2]));
        }
      }
    }

    ofstream out(output_file);

    out << "p tw " << vertex_id << " " << edges.size() << endl;
    
    for (const jet::Attr& var : mgr.var_partition.sys_vars())
    {
      out << "c " << vertices[var] << " " << mgr.var_partition.get_name(var)
           << endl;
    }

    for (const SymbolicDFA& dfa : symbolic_dfas)
    {
      out << "c DFA #" << dfa.index() << ":";

      for (const jet::Attr& var : dfa.next_state_vars())
      {
        out << " " << vertices[var];
      }

      out << endl;
    }
    
    for (const pair<size_t, size_t>& edge : edges)
    {
      out << edge.first << " " << edge.second << endl;
    }

    return 0;
}
